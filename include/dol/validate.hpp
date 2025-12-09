/**
 * @file include/dol/validate.hpp
 * @brief DOL header validation.
 * 
 */

#ifndef DOL_VALIDATE
#define DOL_VALIDATE

#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include "util/log.hpp"

namespace freecube::dol {

    /**
     * @brief The DOL header for GameCube executables
     */
    struct DolHeader {
        static constexpr size_t NUM_TEXT = 7;
        static constexpr size_t NUM_DATA = 11;

        std::array<uint32_t, NUM_TEXT> textOffsets{};
        std::array<uint32_t, NUM_DATA> dataOffsets{};
    
        std::array<uint32_t, NUM_TEXT> textAddrs{};
        std::array<uint32_t, NUM_DATA> dataAddrs{};

        std::array<uint32_t, NUM_TEXT> textSizes{};
        std::array<uint32_t, NUM_DATA> dataSizes{};

        uint32_t bssAddr = 0;
        uint32_t bssSize = 0;
        uint32_t entryPoint = 0;
    };

    enum class DolStatus {
        OK,
        FileTooSmall,
        SectionOutOfBounds,
        EntryPointInvalid
    };

    struct DolValidationResult {
        DolStatus status;
        std::string message;
    };
    
    DolValidationResult validateDol(const DolHeader& hdr, size_t filesize);

    bool readDolHeader(const std::vector<uint8_t>& data, DolHeader& out);
}

#endif