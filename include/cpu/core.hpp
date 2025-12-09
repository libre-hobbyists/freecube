#pragma once

#include <cstdint>
#include <array>

namespace freecube::cpu {

    /**
     * @brief PowerPC 750CL CPU state
     * 
     * Represents the complete architectural state (approx) of the GameCube's CPU.
     * 
     * @note Inaccuracies should be repored ASAP to prevent poor performance of content!
     */
    struct CPUState {
        std::array<uint32_t, 32> gpr;   //< General purpose registers (32x32-bit)

        uint32_t pc;                    //< Program Counter
        uint32_t lr;                    //< Link Register
        uint32_t ctr;                   //< Count Register
        uint32_t xer;                   //< Fixed-Point Exception Register
        uint32_t cr;                    //< Condition Register (8x4-bit fields)

        std::array<double, 32> fpr;     //< General FloatingPoint Registers (32x64-bit)
        uint32_t fpscr;                 //< FloatingPoint Status and Control Register

        uint32_t msr;                   //< Machine State Register

        std::array<uint32_t, 16> sr;    //< Segment Registers (16x32-bit)

        uint32_t spr[1024];             //< Sparse array for various other SPRs we may need

        /**
         * @brief Init CPU to power-on state
         */
        void reset() {
            gpr.fill(0);
            fpr.fill(0.0);
            sr.fill(0);
            
            pc = 0;
            lr = 0;
            ctr = 0;
            xer = 0;
            cr = 0;
            fpscr = 0;
            msr = 0;
            
            for (auto& s : spr) s = 0;
        }
    };

    /**
     * @brief Decoded PowerPC instruction
     */
    struct Instruction {
        uint32_t raw;

        uint8_t opcode;         //< Primary opcode
        uint8_t rD;             //< Destination reg
        uint8_t rA;             //< Source reg A
        uint8_t rB;             //< Source reg B
        int16_t simm;           //< Signed immediate val
        uint16_t uimm;          //< Unsigned immediate val
        uint16_t extended;      //< Extended opcode (varies on instr)

        /**
         * @todo Expand on this as we need!
         */
    };

    /**
     * @brief Decode a raw 32-bit instruction word
     * 
     * @param raw The raw instruction bytes (big-endian)
     * @return Decoded instruction
     */
    Instruction decode(uint32_t raw);

    /**
     * @brief Execute a single instruction
     * 
     * @param cpu CPU state to modify
     * @param inst Decoded instruction to execute
     */
    void execute(CPUState& cpu, const Instruction& inst);
}