#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <optional>
#include <stdexcept>
#include <algorithm>
#include <cstdio>

#include "util/log.hpp"

namespace freecube::ISOLoader {

    class ISOImage {
    public:
        explicit ISOImage(const std::string &path) {
            load_file(path);
            validate();
        }

        const std::vector<std::uint8_t>& data() const noexcept {
            return m_data;
        }

        std::size_t size() const noexcept {
            return m_data.size();
        }

        /**
         * @brief Extract a file from the GameCube ISO by filename or path.
         *
         * Accepts:
         *  - "main.dol"
         *  - "sys/main.dol"
         *  - "/sys/main.dol"
         *
         * NOTE: GameCube FST stores only final name components in entries;
         *       we reconstruct parent directories by walking enclosing directory entries.
         */
        std::optional<std::vector<std::uint8_t>>
        extract_file(const std::string& path) const
        {
            // Put per-file dump under a very verbose gate, that being debug/trace only
            if(util::LogCFG::min_level < util::LogLevel::FC_DEBUG) {
                dump_fst_header();
                dump_fst();
            }

            LOG_TRACE("Extracting file: ", path);

            if (m_data.size() < 0x430) {
                LOG_ERROR("ISO too small for FST");
                return std::nullopt;
            }

            // Normalise requested path: remove leading slash
            std::string clean = path;
            if (!clean.empty() && clean.front() == '/')
                clean.erase(0,1);

            // Split by '/'
            std::vector<std::string> comps;
            {
                std::size_t pos = 0;
                while (pos < clean.size()) {
                    auto nxt = clean.find('/', pos);
                    if (nxt == std::string::npos) {
                        comps.emplace_back(clean.substr(pos));
                        break;
                    } else {
                        comps.emplace_back(clean.substr(pos, nxt - pos));
                        pos = nxt + 1;
                    }
                }
            }

            if (comps.empty()) {
                LOG_WARN("Empty filename requested.");
                return std::nullopt;
            }

            const std::string target_name = comps.back();

            // Header offsets (big-endian)
            uint32_t fst_offset = read_be32(m_data.data() + 0x424);
            uint32_t fst_size   = read_be32(m_data.data() + 0x428);

            LOG_DEBUG("FST offset: ", fst_offset);
            LOG_DEBUG("FST size: ", fst_size);

            if (fst_offset + fst_size > m_data.size()) {
                LOG_ERROR("FST outside ISO bounds");
                return std::nullopt;
            }

            const uint8_t* entries_base = m_data.data() + fst_offset;

            // Read root entry's file_size field (big-endian) -> number of entries
            uint32_t entry_count = read_be32(entries_base + 8);

            LOG_DEBUG("FST entry count: ", entry_count);

            // Sanity check
            if (entry_count == 0 || static_cast<uint64_t>(entry_count) * 12 > fst_size) {
                LOG_ERROR("FST entry count invalid or out of bounds");
                return std::nullopt;
            }

            // string table begins after entry_count * 12 bytes
            const char* string_table = reinterpret_cast<const char*>(entries_base + entry_count * 12);

            // Helper to read an entry (name+flags, offset, size) in BE form
            auto read_entry = [&](uint32_t idx) -> std::tuple<uint32_t,uint32_t,uint32_t> {
                const uint8_t* p = entries_base + static_cast<size_t>(idx) * 12;
                uint32_t name_off_flags = read_be32(p + 0);
                uint32_t off            = read_be32(p + 4);
                uint32_t len            = read_be32(p + 8);
                return { name_off_flags, off, len };
            };

            // Helper: reconstruct path components for a given entry index by walking parent directories
            auto build_entry_path = [&](uint32_t idx)->std::vector<std::string> {
                std::vector<std::string> parts;

                auto [nf_be, off, len] = read_entry(idx);
                // correct decode: flags in MSB, name offset in low 24 bits
                uint8_t flags = static_cast<uint8_t>((nf_be >> 24) & 0xFF);
                uint32_t name_off = nf_be & 0x00FFFFFFu;
                const char* name = string_table + name_off;
                parts.emplace_back(name);

                // Walk backward to find directories that enclose idx
                for (int32_t j = static_cast<int32_t>(idx) - 1; j >= 0; --j) {
                    auto [nfj_be, off_j, len_j] = read_entry(static_cast<uint32_t>(j));
                    uint8_t flagsj = static_cast<uint8_t>((nfj_be >> 24) & 0xFF);
                    if ((flagsj & 1) == 0)
                        continue; // not a directory

                    uint32_t dir_end = len_j; // for directories, file_size stores last index
                    if (dir_end > idx) {
                        uint32_t name_off_j = nfj_be & 0x00FFFFFFu;
                        const char* name_j = string_table + name_off_j;
                        parts.emplace_back(name_j);
                    }
                }

                std::reverse(parts.begin(), parts.end());
                return parts;
            };

            // Walk entries and find candidate files named target_name
            for (uint32_t i = 0; i < entry_count; ++i) {
                auto [nf_be, off, len] = read_entry(i);

                // decode correctly: MSB = flags/type, low 24 bits = name offset
                uint8_t flags = static_cast<uint8_t>((nf_be >> 24) & 0xFF);
                uint32_t name_off = nf_be & 0x00FFFFFFu;
                bool is_dir = (flags & 1) != 0;
                const char* name = string_table + name_off;

                // only care about files
                if (is_dir)
                    continue;

                if (target_name != name)
                    continue;

                // If user requested a path with parents, validate parent chain
                if (comps.size() > 1) {
                    auto entry_parts = build_entry_path(i);

                    if (entry_parts.size() < comps.size())
                        continue;

                    bool ok = true;
                    size_t offset_idx = entry_parts.size() - comps.size();
                    for (size_t k = 0; k < comps.size(); ++k) {
                        if (entry_parts[offset_idx + k] != comps[k]) {
                            ok = false;
                            break;
                        }
                    }
                    if (!ok) continue;
                }

                uint32_t file_off = off;
                uint32_t file_sz  = len;

                LOG_INFO("Found file ", name, " offset=", file_off, " size=", file_sz);

                if (static_cast<uint64_t>(file_off) + file_sz > m_data.size()) {
                    LOG_ERROR("File exceeds ISO bounds");
                    return std::nullopt;
                }

                return std::vector<std::uint8_t>(
                    m_data.begin() + file_off,
                    m_data.begin() + file_off + file_sz
                );
            }

            LOG_WARN("File not found: ", path);
            return std::nullopt;
        }

        void dump_fst() const {
            LOG_INFO("---- BEGIN FST DUMP ----");

            uint32_t fst_offset = read_be32(m_data.data() + 0x424);
            uint32_t fst_size   = read_be32(m_data.data() + 0x428);

            const uint8_t* entries_base = m_data.data() + fst_offset;

            uint32_t entry_count = read_be32(entries_base + 8);

            const char* string_table = reinterpret_cast<const char*>(entries_base + entry_count * 12);

            for (uint32_t i = 0; i < entry_count; ++i) {

                uint32_t nf_be   = read_be32(entries_base + static_cast<size_t>(i) * 12 + 0);
                uint32_t off_be  = read_be32(entries_base + static_cast<size_t>(i) * 12 + 4);
                uint32_t size_be = read_be32(entries_base + static_cast<size_t>(i) * 12 + 8);

                // decode correctly
                uint8_t flags    = static_cast<uint8_t>((nf_be >> 24) & 0xFF);
                uint32_t name_off = nf_be & 0x00FFFFFFu;
                const char* name = string_table + name_off;

                if (flags & 1) {
                    LOG_TRACE("[DIR ] ", name,
                             " first=", i,
                             " last=", size_be);
                } else {
                    LOG_TRACE("[FILE] ", name,
                             " offset=0x", std::hex, off_be,
                             " size=0x", size_be, std::dec);
                }
            }
        
            LOG_INFO("---- END FST DUMP ----");
        }

        std::vector<uint8_t> get_dol() const {
            if (m_data.size() < 0x424) {
                throw std::runtime_error("ISO too small to contain DOL offset");
            }

            // Read DOL offset from disc header at 0x420
            uint32_t dol_offset = read_be32(m_data.data() + 0x420);

            LOG_INFO("DOL offset from header: ", dol_offset);

            // DOL header is 0x100 bytes, read it first to get total size
            if (dol_offset + 0x100 > m_data.size()) {
                throw std::runtime_error("DOL offset out of bounds");
            }

            // For now, just return the first ~4MB or calculate actual size from DOL header
            uint32_t dol_size = 0x400000; // 4MB should be enough for most DOLs

            if (dol_offset + dol_size > m_data.size()) {
                dol_size = m_data.size() - dol_offset;
            }

            return std::vector<uint8_t>(
                m_data.begin() + dol_offset,
                m_data.begin() + dol_offset + dol_size
            );
        }

    private:
        std::vector<std::uint8_t> m_data;

        // We read raw bytes to avoid endian/UB issues.
        struct FSTEntry {
            std::uint32_t name_offset_flags;
            std::uint32_t file_offset;
            std::uint32_t file_size;
        };

        static std::uint32_t read_be32(const std::uint8_t* p) {
            return (static_cast<std::uint32_t>(p[0]) << 24) |
                   (static_cast<std::uint32_t>(p[1]) << 16) |
                   (static_cast<std::uint32_t>(p[2]) << 8)  |
                    static_cast<std::uint32_t>(p[3]);
        }

        void load_file(const std::string &path) {
            std::ifstream f(path, std::ios::binary | std::ios::ate);
            if (!f) {
                LOG_ERROR("ISO image not loaded!");
                throw std::runtime_error("ISOImage: failed to open file: " + path);
            }

            std::streamsize sz = f.tellg();
            if (sz <= 0) {
                LOG_ERROR("ISO image is empty!");
                throw std::runtime_error("ISOImage: File is empty: " + path);
            }

            m_data.resize(static_cast<std::size_t>(sz));
            f.seekg(0, std::ios::beg);

            if (!f.read(reinterpret_cast<char*>(m_data.data()), sz)) {
                LOG_ERROR("Failed to read ISO image!");
                throw std::runtime_error("ISOImage: failed to read file: " + path);
            }

            LOG_TRACE("ISO loaded OK.");
        }

        void validate() {
            LOG_TRACE("Validating ISO image...");

            constexpr std::size_t sector = 0x8000;

            if (m_data.size() % sector != 0) {
                LOG_ERROR("ISO is not a valid size!");
                throw std::runtime_error("ISOImage: invalid size (not a multiple of 32kb)");
            }
        
            // GameCube discs start with a 6-byte game ID
            // Format: System(1) + GameCode(2) + Region(1) + Maker(2)
            // We just check that it's ASCII printable characters
            // If not, **then** we panic.

            if (m_data.size() < 0x20) {
                LOG_ERROR("Too small for boot.bin validation");
                throw std::runtime_error("ISOImage: too small for boot.bin validation");
            }
        
            // Read the game ID (first 6 bytes)
            std::string game_id(reinterpret_cast<const char*>(m_data.data()), 6);
            LOG_INFO("Game ID: ", game_id);

            // Check if it starts with 'G' (GameCube ROM) or 'D' (Demofile(?))
            if (m_data[0] != 'G' && m_data[0] != 'D') {
                LOG_ERROR("Invalid GameCube disc ID! Expected 'G' or 'D', got: ", (char)m_data[0]);
                throw std::runtime_error("ISOImage: invalid boot.bin magic");
            }

            // Verify all 6 bytes are printable ASCII
            for (std::size_t i = 0; i < 6; ++i) {
                if (m_data[i] < 0x20 || m_data[i] > 0x7E) {
                    LOG_ERROR("Invalid character in game ID at position ", i);
                    throw std::runtime_error("ISOImage: invalid game ID");
                }
            }
        
            LOG_TRACE("ISO validation OK.");
        }

        void dump_bytes(const char* label, uint32_t offset, uint32_t count) const {
            LOG_INFO(label, " @ 0x", std::hex, offset, std::dec);
            std::string line;
            for (uint32_t i = 0; i < count; i++) {
                char buf[4];
                snprintf(buf, sizeof(buf), "%02X ", m_data[offset + i]);
                line += buf;
            }
            LOG_INFO(line);
        }

        void dump_fst_header() const {
            uint32_t fst_offset = read_be32(m_data.data() + 0x424);
            uint32_t fst_size   = read_be32(m_data.data() + 0x428);
        
            LOG_INFO("fst_offset = 0x", std::hex, fst_offset, "  fst_size = 0x", fst_size, std::dec);
        
            dump_bytes("Header @ 0x420", 0x420, 32);
            dump_bytes("FST start", fst_offset, 32);
        }
    };

} // namespace freecube::ISOLoader
