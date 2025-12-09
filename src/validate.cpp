#include "dol/validate.hpp"
#include <cstring>

namespace freecube::dol {

    /**
     * @brief PowerPC CPUs are big-endian, we cannot assume the emulator host is also BE.
     */
    static uint32_t read_be32(const uint8_t *p) {
        return (uint32_t(p[0]) << 24)  |
                (uint32_t(p[1]) << 16) |
                (uint32_t(p[2]) << 8)  |
                uint32_t(p[3]);
    }

    bool readDolHeader(const std::vector<uint8_t> &data, DolHeader &out) {
        if (data.size() < 0x100) {
            LOG_CRITICAL("DOL header is smaller than 0x100!");
            return false;
        }

        const uint8_t *p = data.data();

        for (size_t i = 0; i < DolHeader::NUM_TEXT; i++)
            out.textOffsets[i] = read_be32(p + 0x00 + (i * 4));

        for (size_t i = 0; i < DolHeader::NUM_DATA; i++)
            out.dataOffsets[i] = read_be32(p + 0x1C + (i * 4));

        for (size_t i = 0; i < DolHeader::NUM_TEXT; i++)
            out.textAddrs[i] = read_be32(p + 0x48 + (i * 4));

        for (size_t i = 0; i < DolHeader::NUM_DATA; i++)
            out.dataAddrs[i] = read_be32(p + 0x64 + (i * 4));

        for (size_t i = 0; i < DolHeader::NUM_TEXT; i++)
            out.textSizes[i] = read_be32(p + 0x90 + (i * 4));

        for (size_t i = 0; i < DolHeader::NUM_DATA; i++)
            out.dataSizes[i] = read_be32(p + 0xAC + (i * 4));

        out.bssAddr     = read_be32(p + 0xD8);
        out.bssSize     = read_be32(p + 0xDC);
        out.entryPoint  = read_be32(p + 0xE0);

        LOG_TRACE("Header looks OK.");
        return true;
    }

    DolValidationResult validateDol(const DolHeader &hdr, size_t filesize) {
        // Offsets && sizes must be within file
        auto check = [&](uint32_t off, uint32_t size) {
            if (size == 0) return true;
            if (off == 0) return false;
            if (off >= filesize) return false;
            if (off + size > filesize) return false;
            return true;
        };

        // NUM_TEXT
        for (size_t i = 0; i < DolHeader::NUM_TEXT; i++) {
            if (!check(hdr.textOffsets[i], hdr.textSizes[i])) {
                return {DolStatus::SectionOutOfBounds,
                "Text Section \"" + std::to_string(i) + "\" is invalid."};
            }
        }
        LOG_TRACE("NUM_TEXT is OK.");

        // NUM_DATA
        for (size_t i = 0; i < DolHeader::NUM_DATA; i++) {
            if (!check(hdr.dataOffsets[i], hdr.dataSizes[i])) {
                return {DolStatus::SectionOutOfBounds,
                "Data Section \"" + std::to_string(i) + "\" is invalid."};
            }
        }
        LOG_TRACE("NUM_DATA is OK.");

        // Entrypoint has to be in a text section, not data
        bool entry_ok = false;
        for (size_t i = 0; i < DolHeader::NUM_TEXT; i++) {
            uint32_t start = hdr.textAddrs[i];
            uint32_t end = start + hdr.textSizes[i];

            if (hdr.entryPoint >= start && hdr.entryPoint < end) {
                LOG_TRACE("Entrypoint is OK.");
                entry_ok = true;
                break;
            }
        }

        if (!entry_ok)
            LOG_TRACE("Entrypoint is not valid.");
            return {DolStatus::EntryPointInvalid, "Entrypoint is not in a text section."};

        LOG_TRACE("DOL is OK.");
        return {DolStatus::OK, "Valid DOL file."};
    }

}   // namespace freecube::dol