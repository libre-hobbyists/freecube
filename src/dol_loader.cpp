#include "dol/dol_loader.hpp"
#include "util/log.hpp"
#include <stdexcept>

namespace freecube::dol {
    uint32_t DOLLoader::be32(const uint8_t *p) {
        return (p[0] << 24) |
                (p[1] << 16) |
                (p[2] << 8)  |
                p[3];
    }

    DOLLoader::DOLLoader(const std::vector<uint8_t> &bytes) {
        if (bytes.size() < 0x100) {
            LOG_ERROR("Header is too small!");
            throw std::runtime_error("DOL: Header too small.");
        }

        parse_header(bytes.data());
        load_sections(bytes);
    }

    void DOLLoader::parse_header(const uint8_t *header) {
        // text offsets (0x00 -> 0x1B)
        for (int i = 0; i < 7; i++) {
            m_image.text[i].file_offset = be32(header + 0x00 + i * 4);
        }

        // data offsets (0x1C -> 0x3B)
        for (int i = 0; i < 11; i++) {
            m_image.data[i].file_offset = be32(header + 0x1C + i * 4);
        }

        // text load addresses (0x48 -> 0x63)
        for (int i = 0; i < 7; i++) {
            m_image.text[i].load_address = be32(header + 0x48 + i * 4);
        }

        // data load addresses (0x64 -> 0x8B)
        for (int i = 0; i < 11; i++) {
            m_image.data[i].load_address = be32(header + 0x64 + i * 4);
        }

        // text sizes (0x90 -> 0xAB)
        for (int i = 0; i < 7; i++) {
            m_image.text[i].size = be32(header + 0x90 + i * 4);
        }

        // data sizes (0xAC -> 0xD3)
        for (int i = 0; i < 11; i++) {
            m_image.data[i].size = be32(header + 0xAC + i * 4);
        }

        // bss (0xD8 -> 0xDF)
        m_image.bss_address = be32(header + 0xD8);
        m_image.bss_size = be32(header + 0xDC);

        // entry point (0xE0)
        m_image.entry_point = be32(header + 0xE0);

        LOG_DEBUG("DOL entrypoint: ", m_image.entry_point);
        LOG_DEBUG("BSS Start: ", m_image.bss_address);
        LOG_DEBUG("BSS End: ", m_image.bss_address + m_image.bss_size);
        LOG_DEBUG("BSS Size: ", m_image.bss_size);
    }

    void DOLLoader::load_sections(const std::vector<uint8_t> &bytes) {
        auto load = [&](Section &sec, uint32_t offset, uint32_t size, uint32_t load_addr) {
            if (offset == 0 || size == 0)
                return;

            if (offset + size > bytes.size()) {
                LOG_ERROR("Section ran out of bounds!");
                throw std::runtime_error("DOL: Section out of bounds");
            }

            sec.data.assign(bytes.begin() + offset, bytes.begin() + offset + size);
            sec.size = size;
            sec.load_address = load_addr;

            LOG_TRACE("Loaded section", load_addr);
            LOG_TRACE("Size: ", size);
        };

        // loop through text && data sections
        for (int i = 0; i < 7; i++) {
            load(m_image.text[i], 
                 m_image.text[i].file_offset,
                 m_image.text[i].size,
                 m_image.text[i].load_address);
        }

        for (int i = 0; i < 11; i++) {
            load(m_image.data[i],
                 m_image.data[i].file_offset,
                 m_image.data[i].size,
                 m_image.data[i].load_address);
        }
    }

} // namespace freecube::dol