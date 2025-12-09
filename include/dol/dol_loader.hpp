#pragma once

#include <cstdint>
#include <vector>
#include <array>

namespace freecube::dol {
    struct Section {
        uint32_t file_offset;
        uint32_t load_address;
        uint32_t size;
        std::vector<uint8_t> data;
    };

    struct DOLImage {
        std::array<Section, 7> text;
        std::array<Section, 11> data;

        uint32_t bss_address;
        uint32_t bss_size;
        uint32_t entry_point;
    };

    class DOLLoader {
        public:
            explicit DOLLoader(const std::vector<uint8_t> &bytes);

            const DOLImage &image() const { return m_image; }

        private:
            DOLImage m_image;

            static uint32_t be32(const uint8_t *p);
            void parse_header(const uint8_t *header);
            void load_sections(const std::vector<uint8_t> &bytes);
    };
}