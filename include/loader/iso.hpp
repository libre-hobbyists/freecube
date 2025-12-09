#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include "util/log.hpp"

namespace freecube::ISOLoader {

    class ISOImage {
        public:
            explicit ISOImage(const std::string &path) {
                load_file(path);
                validate();
            }

            const std::vector<std::uint8_t> &data() const noexcept {
                return m_data;
            }

            std::size_t size() const noexcept {
                return m_data.size();
            }

        private:
            std::vector<std::uint8_t> m_data;

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

                // ~32kb per GameCube sector
                constexpr std::size_t sector = 0x8000;

                if (m_data.size() % sector != 0) {
                    LOG_ERROR("ISO is not a valid size!");
                    throw std::runtime_error("ISOImage: invalid size (not a multiple of 32kb)");
                }

                // boot.bin offset at 0x0000
                // magic = "GLZE01" : 47 5A 4C 45 30 31
                constexpr std::uint8_t magic[] = { 0x47,0x5A,0x4C,0x45,0x30,0x31 };
                LOG_TRACE("GameCube Magic: \"GLZE01\" or 47,5A,4C,45,30,31");

                if (m_data.size() < 0x20) {
                    LOG_ERROR("Too small to validate boot.bin!");
                    throw std::runtime_error("ISOImage: too small for boot.bin validation");
                }
                
                for (std::size_t i = 0; i < sizeof(magic); ++i) {
                    LOG_DEBUG("Expected: ", magic[i]);
                    LOG_DEBUG("Found: ", m_data[i]);

                    if (m_data[i] != magic[i]) {
                        LOG_ERROR("Invalid boot.bin magic number! (Not a GameCube ISO file)");
                        throw std::runtime_error("ISOImage: invalid boot.bin magic (not a GameCube ISO)");
                    }
                }

            }
    };

} // namespace freecube:ISOLoader