#include "isa.h"
#include "cpu.h"
#include <iostream>
#include <algorithm>

void cpu::instruction_decode(uint32_t instruction)
{
    uint8_t memory_instructions[] = {INST_STA1};
    // decode instruction
    uint8_t opcode = (instruction & 0x000000FF);
    // call execute
    if (std::find(std::begin(memory_instructions), std::end(memory_instructions), opcode) != std::end(memory_instructions))
    {
        execute_memory_instructions(opcode, instruction);
    }
    else
    {
        execute(opcode, instruction);
    }
}

void cpu::execute(uint8_t opcode, uint32_t instruction)
{

    if (opcode == INST_ABS && ((instruction & 0x0FF00000) >> 20 == 0x1C))
    {
        int b = (instruction & 0x0000F000) >> 12; // read instruction[12:15]
        int c = (instruction & 0xF0000000) >> 28; // read instruction[28:31]
        uint32_t abs_result = (D[b] >= 0) ? D[b] : (0 - D[b]);
        D[c] = abs_result;

        // update PSW
        uint32_t overflow = (abs_result > 0x7FFFFFFF) || (abs_result < -0x80000000);
        // update the PSW.V bit
        PSW = overflow ? PSW | (1 << 30) : PSW & ~(1 << 30);
        // update the PSW.SV bit
        if (overflow)
        {
            PSW = PSW | (1 << 29);
        }

        // update the PSW.AV bit
        uint32_t advanced_overflow = (abs_result & 0x80000000) ^ (abs_result & 0x40000000); // result[31] ^ result[30];
        PSW = advanced_overflow ? PSW | (1 << 28) : PSW & ~(1 << 28);
        // update the PSW.SAV bit
        if (advanced_overflow)
        {
            PSW = PSW | (1 << 27);
        }
        std::cout << "instruction abs";
        PC += 1;
    }

    // AND instruction
    //  else if(opcode == INST_AND_RC)
    //  {
    //      int a = (instruction & 0x00000F00)>>8;
    //      int const = (instruction & 0x001FF000)>>12;
    //      int c = (instruction & 0xF0000000)>>28;

    //     D[c] = D[a] & const;
    //     std::cout << "instruction and rc" << D[c];
    // }
    // else if(opcode == INST_AND_RR)
    // {
    //     int a = (instruction & 0x00000F00)>>8;
    //     int b = (instruction & 0x0000F000)>>12;

    //     D[c] = D[a] & D[b];
    //     std::cout << "instruction and rr" << D[c];
    // }
    // else if(opcode == INST_AND_SC)
    // {
    //     int const = (instruction & 0x0000FF00)>>8;

    //     D[15] = D[15] & const;
    //     std::cout << "instruction and sc" << D[15];
    // }
    // else if(opcode == INST_AND_SR)
    // {
    //     int a = (instruction & 0x0F00)>>8;
    //     int b = (instruction & 0xF000)>>12;

    //     D[a] = D[a] & D[b];
    //     std::cout << "instruction and sr" << D[a];
    //     PC += 1;
    // }
    /*
     * ABS.B
     * Absolute value packed byte
     */
    else if (opcode == INST_ABS && ((instruction & 0x0FF00000) >> 20 == 0x5C))
    {
        int b = (instruction & 0x0000F000) >> 12; // read instruction[12:15]
        int c = (instruction & 0xF0000000) >> 28; // read instruction[28:31]
        uint8_t result_byte3 = ((D[b] & 0xFF000000) >> 24) >= 0 ? ((D[b] & 0xFF000000) >> 24) : (0 - ((D[b] & 0xFF000000) >> 24));
        uint8_t result_byte2 = ((D[b] & 0x00FF0000) >> 16) >= 0 ? ((D[b] & 0x00FF0000) >> 16) : (0 - ((D[b] & 0x00FF0000) >> 16));
        uint8_t result_byte1 = ((D[b] & 0x0000FF00) >> 8) >= 0 ? ((D[b] & 0x0000FF00) >> 8) : (0 - ((D[b] & 0x0000FF00) >> 8));
        uint8_t result_byte0 = (D[b] & 0x000000FF) >= 0 ? (D[b] & 0x000000FF) : (0 - (D[b] & 0x000000FF));
        D[c] = (result_byte3 << 24) | (result_byte2 << 16) | (result_byte1 << 8) | result_byte0;

        // update PSW
        uint32_t overflow = (result_byte3 > 0x7F) || (result_byte3 < -0x80) || (result_byte2 > 0x7F) || (result_byte2 < -0x80) || (result_byte1 > 0x7F) || (result_byte1 < -0x80) || (result_byte0 > 0x7F) || (result_byte0 < -0x80);
        // update the PSW.V bit
        PSW = overflow ? PSW | (1 << 30) : PSW & ~(1 << 30);
        // update the PSW.SV bit
        if (overflow)
        {
            PSW = PSW | (1 << 29);
        }
        // update the PSW.AV bit
        uint8_t aov_byte3 = ((result_byte3 & 0x80) ^ (result_byte3 & 0x40)) >> 6; // result[7] ^ result[6];
        uint8_t aov_byte2 = ((result_byte2 & 0x80) ^ (result_byte2 & 0x40)) >> 6; // result[7] ^ result[6];
        uint8_t aov_byte1 = ((result_byte1 & 0x80) ^ (result_byte1 & 0x40)) >> 6; // result[7] ^ result[6];
        uint8_t aov_byte0 = ((result_byte0 & 0x80) ^ (result_byte0 & 0x40)) >> 6; // result[7] ^ result[6];
        uint32_t advanced_overflow = aov_byte3 || aov_byte2 || aov_byte1 || aov_byte0;
        PSW = advanced_overflow ? PSW | (1 << 28) : PSW & ~(1 << 28);
        // update the PSW.SAV bit
        if (advanced_overflow)
        {
            PSW = PSW | (1 << 27);
        }
        PC += 1;
    }
    else if (opcode == INST_ABS && ((instruction & 0x0FF00000) >> 20 == 0x7C))
    {
        int b = (instruction & 0x0000F000) >> 12; // read instruction[12:15]
        int c = (instruction & 0xF0000000) >> 28; // read instruction[28:31]
        uint16_t result_word1 = ((D[b] & 0xFFFF0000) >> 16) >= 0 ? ((D[b] & 0xFFFF0000) >> 16) : (0 - ((D[b] & 0xFFFF0000) >> 16));
        uint16_t result_word0 = (D[b] & 0x0000FFFF) >= 0 ? (D[b] & 0x0000FFFF) : (0 - (D[b] & 0x0000FFFF));
        D[c] = (result_word1 << 16) | result_word0;

        // update PSW
        uint32_t overflow = (result_word1 > 0x7FFF) || (result_word1 < -0x8000) || (result_word0 > 0x7FFF) || (result_word0 < -0x8000);
        // update the PSW.V bit
        PSW = overflow ? PSW | (1 << 30) : PSW & ~(1 << 30);
        // update the PSW.SV bit
        if (overflow)
        {
            PSW = PSW | (1 << 29);
        }
        // update the PSW.AV bit
        uint16_t aov_word1 = ((result_word1 & 0x8000) ^ (result_word1 & 0x4000)) >> 14; // result[15] ^ result[14];
        uint16_t aov_word0 = ((result_word0 & 0x8000) ^ (result_word0 & 0x4000)) >> 14; // result[15] ^ result[14];
        uint32_t advanced_overflow = aov_word1 || aov_word0;
        PSW = advanced_overflow ? PSW | (1 << 28) : PSW & ~(1 << 28);
        // update the PSW.SAV bit
        if (advanced_overflow)
        {
            PSW = PSW | (1 << 27);
        }
        PC += 1;
    }
    else if (opcode == INST_ABSDIF_CONST && ((instruction & 0x07F00000) >> 21 == 0x0E))
    {
        int a = (instruction & 0x00000F00) >> 8;  // read instruction[8:11]
        int c = (instruction & 0xF0000000) >> 28; // read instruction[28:31]
        uint32_t temp = cpu::rtl.sign_ext(instruction & 0x001FF000);
        uint32_t result = (D[a] > temp) ? (D[a] - temp) : (temp - D[a]);
        D[c] = result;

        // update PSW
        uint32_t overflow = (result > 0x7FFFFFFF) || (result < -0x80000000);
        // update the PSW.V bit
        PSW = overflow ? PSW | (1 << 30) : PSW & ~(1 << 30);
        // update the PSW.SV bit
        if (overflow)
        {
            PSW = PSW | (1 << 29);
        }
        // update the PSW.AV bit
        uint32_t aov = ((result & 0x80000000) ^ (result & 0x40000000)) >> 30; // result[31] ^ result[30];
        PSW = aov ? PSW | (1 << 28) : PSW & ~(1 << 28);
        // update the PSW.SAV bit
        if (aov)
        {
            PSW = PSW | (1 << 27);
        }
        PC += 1;
    }
    else if (opcode == INST_ABS && ((instruction & 0x0FF00000) >> 20 == 0x0E))
    {
        int a = (instruction & 0x00000F00) >> 8;  // read instruction[8:11]
        int b = (instruction & 0x0000F000) >> 12; // read instruction[12:15]
        int c = (instruction & 0xF0000000) >> 28; // read instruction[28:31]
        uint32_t result = (D[a] > D[b]) ? (D[a] - D[b]) : (D[b] - D[a]);
        D[c] = result;

        // update PSW
        uint32_t overflow = (result > 0x7FFFFFFF) || (result < -0x80000000);
        // update the PSW.V bit
        PSW = overflow ? PSW | (1 << 30) : PSW & ~(1 << 30);
        // update the PSW.SV bit
        if (overflow)
        {
            PSW = PSW | (1 << 29);
        }
        // update the PSW.AV bit
        uint32_t aov = ((result & 0x80000000) ^ (result & 0x40000000)) >> 30; // result[31] ^ result[30];
        PSW = aov ? PSW | (1 << 28) : PSW & ~(1 << 28);
        // update the PSW.SAV bit
        if (aov)
        {
            PSW = PSW | (1 << 27);
        }
        PC += 1;
    }
    /*
     * ABSDIF.B
     * Absolute Value of Difference packed byte
     */
    else if (opcode == INST_ABS && ((instruction & 0x0FF00000) >> 20 == 0x4E))
    {
        int a = (instruction & 0x00000F00) >> 8;  // read instruction[8:11]
        int b = (instruction & 0x0000F000) >> 12; // read instruction[12:15]
        int c = (instruction & 0xF0000000) >> 28; // read instruction[28:31]
        uint8_t result_byte3 = (D[a] & 0xFF000000) > (D[b] & 0xFF000000) ? ((D[a] & 0xFF000000) - (D[b] & 0xFF000000)) : ((D[b] & 0xFF000000) - (D[a] & 0xFF000000));
        uint8_t result_byte2 = (D[a] & 0x00FF0000) > (D[b] & 0x00FF0000) ? ((D[a] & 0x00FF0000) - (D[b] & 0x00FF0000)) : ((D[b] & 0x00FF0000) - (D[a] & 0x00FF0000));
        uint8_t result_byte1 = (D[a] & 0x0000FF00) > (D[b] & 0x0000FF00) ? ((D[a] & 0x0000FF00) - (D[b] & 0x0000FF00)) : ((D[b] & 0x0000FF00) - (D[a] & 0x0000FF00));
        uint8_t result_byte0 = (D[a] & 0x000000FF) > (D[b] & 0x000000FF) ? ((D[a] & 0x000000FF) - (D[b] & 0x000000FF)) : ((D[b] & 0x000000FF) - (D[a] & 0x000000FF));
        D[c] = (result_byte3 << 24) | (result_byte2 << 16) | (result_byte1 << 8) | result_byte0;

        // update PSW
        uint32_t overflow = (result_byte3 > 0x7F) || (result_byte3 < -0x80) || ((result_byte2 > 0x7F) || (result_byte2 < -0x80) || ((result_byte1 > 0x7F) || (result_byte1 < -0x80) || ((result_byte0 > 0x7F) || (result_byte0 < -0x80))));
        // update the PSW.V bit
        PSW = overflow ? PSW | (1 << 30) : PSW & ~(1 << 30);
        // update the PSW.SV bit
        if (overflow)
        {
            PSW = PSW | (1 << 29);
        }
        // update the PSW.AV bit
        uint32_t aov_byte3 = ((result_byte3 & 0x80) ^ (result_byte3 & 0x40)) >> 6; // result_byte3[7] ^ result_byte3[6];
        uint32_t aov_byte2 = ((result_byte2 & 0x80) ^ (result_byte2 & 0x40)) >> 6; // result_byte2[7] ^ result_byte2[6];
        uint32_t aov_byte1 = ((result_byte1 & 0x80) ^ (result_byte1 & 0x40)) >> 6; // result_byte1[7] ^ result_byte1[6];
        uint32_t aov_byte0 = ((result_byte0 & 0x80) ^ (result_byte0 & 0x40)) >> 6; // result_byte0[7] ^ result_byte0[6];
        uint32_t aov = aov_byte3 || aov_byte2 || aov_byte1 || aov_byte0;
        PSW = aov ? PSW | (1 << 28) : PSW & ~(1 << 28);
        // update the PSW.SAV bit
        if (aov)
        {
            PSW = PSW | (1 << 27);
        }
        PC += 1;
    }
    /*
     * ABSDIF.H
     * Absolute Value of Difference packed Half-word
     */
    else if (opcode == INST_ABS && ((instruction & 0x0FF00000) >> 20 == 0x6E))
    {
        int a = (instruction & 0x00000F00) >> 8;  // read instruction[8:11]
        int b = (instruction & 0x0000F000) >> 12; // read instruction[12:15]
        int c = (instruction & 0xF0000000) >> 28; // read instruction[28:31]
        uint16_t result_halfword1 = (D[a] & 0xFFFF0000) > (D[b] & 0xFFFF0000) ? ((D[a] & 0xFFFF0000) - (D[b] & 0xFFFF0000)) : ((D[b] & 0xFFFF0000) - (D[a] & 0xFFFF0000));
        uint16_t result_halfword0 = (D[a] & 0x0000FFFF) > (D[b] & 0x0000FFFF) ? ((D[a] & 0x0000FFFF) - (D[b] & 0x0000FFFF)) : ((D[b] & 0x0000FFFF) - (D[a] & 0x0000FFFF));
        D[c] = (result_halfword1 << 16) | result_halfword0;

        // update PSW
        uint32_t overflow = (result_halfword1 > 0x7FFF) || (result_halfword1 < -0x8000) || ((result_halfword0 > 0x7FFF) || (result_halfword0 < -0x8000));
        // update the PSW.V bit
        PSW = overflow ? PSW | (1 << 30) : PSW & ~(1 << 30);
        // update the PSW.SV bit
        if (overflow)
        {
            PSW = PSW | (1 << 29);
        }
        // update the PSW.AV bit
        uint32_t aov_halfword1 = ((result_halfword1 & 0x8000) ^ (result_halfword1 & 0x4000)) >> 14; // result_halfword1[15] ^ result_halfword1[14];
        uint32_t aov_halfword0 = ((result_halfword0 & 0x8000) ^ (result_halfword0 & 0x4000)) >> 14; // result_halfword0[15] ^ result_halfword0[14];
        uint32_t aov = aov_halfword1 || aov_halfword0;
        PSW = aov ? PSW | (1 << 28) : PSW & ~(1 << 28);
        // update the PSW.SAV bit
        if (aov)
        {
            PSW = PSW | (1 << 27);
        }
        PC += 1;
    }
    /*
     * ABSDIFS
     * Absolute Value of Difference with Saturation
     */
    else if (opcode == INST_ABSDIF_CONST && ((instruction & 0x07F00000) >> 21 == 0x0E))
    {
        int a = (instruction & 0x00000F00) >> 8;  // read instruction[8:11]
        int c = (instruction & 0xF0000000) >> 28; // read instruction[28:31]
        uint32_t temp = cpu::rtl.sign_ext(instruction & 0x001FF000);
        uint32_t result = (D[a] > temp) ? (D[a] - temp) : (temp - D[a]);
        D[c] = cpu::rtl.ssov(result, 32);

        // update PSW
        uint32_t overflow = (result > 0x7FFFFFFF) || (result < -0x80000000);
        // update the PSW.V bit
        PSW = overflow ? PSW | (1 << 30) : PSW & ~(1 << 30);
        // update the PSW.SV bit
        if (overflow)
        {
            PSW = PSW | (1 << 29);
        }
        // update the PSW.AV bit
        uint32_t aov = ((result & 0x80000000) ^ (result & 0x40000000)) >> 30; // result[31] ^ result[30];
        PSW = aov ? PSW | (1 << 28) : PSW & ~(1 << 28);
        // update the PSW.SAV bit
        if (aov)
        {
            PSW = PSW | (1 << 27);
        }
        PC += 1;
    }
    else if (opcode == INST_ABS && ((instruction & 0x07F00000) >> 20 == 0x0E))
    {
        int a = (instruction & 0x00000F00) >> 8;  // read instruction[8:11]
        int b = (instruction & 0x0000F000) >> 12; // read instruction[12:15]
        int c = (instruction & 0xF0000000) >> 28; // read instruction[28:31]
        uint32_t result = (D[a] > D[b]) ? (D[a] - D[b]) : (D[b] - D[a]);
        D[c] = cpu::rtl.ssov(result, 32);

        // update PSW
        uint32_t overflow = (result > 0x7FFFFFFF) || (result < -0x80000000);
        // update the PSW.V bit
        PSW = overflow ? PSW | (1 << 30) : PSW & ~(1 << 30);
        // update the PSW.SV bit
        if (overflow)
        {
            PSW = PSW | (1 << 29);
        }
        // update the PSW.AV bit
        uint32_t aov = ((result & 0x80000000) ^ (result & 0x40000000)) >> 30; // result[31] ^ result[30];
        PSW = aov ? PSW | (1 << 28) : PSW & ~(1 << 28);
        // update the PSW.SAV bit
        if (aov)
        {
            PSW = PSW | (1 << 27);
        }
        PC += 1;
    }
    /*
     * ABSDIFS.H
     * Absolute Value of Difference Packed Half-word with Saturation
     */
    else if (opcode == INST_ABS && ((instruction & 0x07F00000) >> 21 == 0x0E))
    {
        int a = (instruction & 0x00000F00) >> 8;  // read instruction[8:11]
        int b = (instruction & 0x0000F000) >> 12; // read instruction[12:15]
        int c = (instruction & 0xF0000000) >> 28; // read instruction[28:31]
        uint16_t result_halfword1 = (D[a] & 0xFFFF0000) > (D[b] & 0xFFFF0000) ? ((D[a] & 0xFFFF0000) - (D[b] & 0xFFFF0000)) : ((D[b] & 0xFFFF0000) - (D[a] & 0xFFFF0000));
        uint16_t result_halfword0 = (D[a] & 0x0000FFFF) > (D[b] & 0x0000FFFF) ? ((D[a] & 0x0000FFFF) - (D[b] & 0x0000FFFF)) : ((D[b] & 0x0000FFFF) - (D[a] & 0x0000FFFF));
        D[c] = cpu::rtl.ssov((result_halfword1 << 16), 16) | cpu::rtl.ssov(result_halfword0, 16);

        // update PSW
        uint32_t overflow = (result_halfword1 > 0x7FFF) || (result_halfword1 < -0x8000) || ((result_halfword0 > 0x7FFF) || (result_halfword0 < -0x8000));
        // update the PSW.V bit
        PSW = overflow ? PSW | (1 << 30) : PSW & ~(1 << 30);
        // update the PSW.SV bit
        if (overflow)
        {
            PSW = PSW | (1 << 29);
        }
        // update the PSW.AV bit
        uint32_t aov_halfword1 = ((result_halfword1 & 0x8000) ^ (result_halfword1 & 0x4000)) >> 14; // result_halfword1[15] ^ result_halfword1[14];
        uint32_t aov_halfword0 = ((result_halfword0 & 0x8000) ^ (result_halfword0 & 0x4000)) >> 14; // result_halfword0[15] ^ result_halfword0[14];
        uint32_t aov = aov_halfword1 || aov_halfword0;
        PSW = aov ? PSW | (1 << 28) : PSW & ~(1 << 28);
        // update the PSW.SAV bit
        if (aov)
        {
            PSW = PSW | (1 << 27);
        }
        PC += 1;
    }
    /*
     * ABSS
     * Absolute Value with Saturation
     */
    else if (opcode == INST_ABS && ((instruction & 0x0FF00000) >> 20 == 0x1D))
    {
        int b = (instruction & 0x0000F000) >> 12; // read instruction[12:15]
        int c = (instruction & 0xF0000000) >> 28; // read instruction[28:31]
        uint32_t result = (D[b] > 0) ? D[b] : (0 - D[b]);
        D[c] = cpu::rtl.ssov(result, 32);

        // update PSW
        uint32_t overflow = (result > 0x7FFFFFFF) || (result < -0x80000000);
        // update the PSW.V bit
        PSW = overflow ? PSW | (1 << 30) : PSW & ~(1 << 30);
        // update the PSW.SV bit
        if (overflow)
        {
            PSW = PSW | (1 << 29);
        }
        // update the PSW.AV bit
        uint32_t aov = ((result & 0x80000000) ^ (result & 0x40000000)) >> 30; // result[31] ^ result[30];
        PSW = aov ? PSW | (1 << 28) : PSW & ~(1 << 28);
        // update the PSW.SAV bit
        if (aov)
        {
            PSW = PSW | (1 << 27);
        }
        PC += 1;
    }
    else if (opcode == INST_ABS && ((instruction & 0x0FF00000) >> 20 == 0x7D))
    {
        int b = (instruction & 0x0000F000) >> 12; // read instruction[12:15]
        int c = (instruction & 0xF0000000) >> 28; // read instruction[28:31]
        uint32_t result_harfword1 = (D[b] & 0xFFFF0000) >= 0 ? (D[b] & 0xFFFF0000) : (0 - (D[b] & 0xFFFF0000));
        uint32_t result_harfword0 = (D[b] & 0x0000FFFF) >= 0 ? (D[b] & 0x0000FFFF) : (0 - (D[b] & 0x0000FFFF));
        D[c] = cpu::rtl.ssov((result_harfword1 << 16), 16) | cpu::rtl.ssov(result_harfword0, 16);

        // update PSW
        uint32_t overflow = (result_harfword1 > 0x7FFF) || (result_harfword1 < -0x8000) || ((result_harfword0 > 0x7FFF) || (result_harfword0 < -0x8000));
        // update the PSW.V bit
        PSW = overflow ? PSW | (1 << 30) : PSW & ~(1 << 30);
        // update the PSW.SV bit
        if (overflow)
        {
            PSW = PSW | (1 << 29);
        }
        // update the PSW.AV bit
        uint32_t aov_halfword1 = ((result_harfword1 & 0x8000) ^ (result_harfword1 & 0x4000)) >> 14; // result_halfword1[15] ^ result_halfword1[14];
        uint32_t aov_halfword0 = ((result_harfword0 & 0x8000) ^ (result_harfword0 & 0x4000)) >> 14; // result_halfword0[15] ^ result_halfword0[14];
        uint32_t aov = aov_halfword1 || aov_halfword0;
        PSW = aov ? PSW | (1 << 28) : PSW & ~(1 << 28);
        // update the PSW.SAV bit
        if (aov)
        {
            PSW = PSW | (1 << 27);
        }
        PC += 1;
    }

    else if (opcode == INST_BISR)
    {
        PC += 1;
    }

    // ######## N #########
    /*
     * NAND  -  Bitwise NAND
     */
    else if (opcode == INST_NAND && ((instruction & 0x07F00000) >> 21 == 0x0E))
    {
        int a = (instruction & 0x00000F00) >> 8;  // read instruction[8:11]
        int c = (instruction & 0xF0000000) >> 28; // read instruction[28:31]
        uint32_t temp = cpu::rtl.sign_ext(instruction & 0x001FF000);
        uint32_t result = (D[a] > temp) ? (D[a] - temp) : (temp - D[a]);
        D[c] = cpu::rtl.ssov(result, 32);

        // update PSW
        uint32_t overflow = (result > 0x7FFFFFFF) || (result < -0x80000000);
        // update the PSW.V bit
        PSW = overflow ? PSW | (1 << 30) : PSW & ~(1 << 30);
        // update the PSW.SV bit
        if (overflow)
        {
            PSW = PSW | (1 << 29);
        }
        // update the PSW.AV bit
        uint32_t aov = ((result & 0x80000000) ^ (result & 0x40000000)) >> 30; // result[31] ^ result[30];
        PSW = aov ? PSW | (1 << 28) : PSW & ~(1 << 28);
        // update the PSW.SAV bit
        if (aov)
        {
            PSW = PSW | (1 << 27);
        }
    }
}

// execute memory instructions
void cpu::execute_memory_instructions(uint8_t opcode, uint32_t instruction)
{
    if (opcode == INST_STA1)
    {
        int a = (instruction & 0x00000F00) >> 8; // read instruction[12:15]
        uint32_t off18 = 0x00000000;
        off18 = (instruction & 0x003F0000) >> 16;                  // read instruction[16:21];
        off18 = off18 | (((instruction & 0xF0000000) >> 28) << 6); // read instruction[28:31];
        off18 = off18 | (((instruction & 0x03C00000) >> 22) << 10);
        off18 = off18 | (((instruction & 0x0000F000) >> 12) << 14);
        uint32_t EA = 0x00000000;
        EA = (off18 & 0x00003FFF) | ((off18 & 0x0003C000) << 28);
        mem.write(EA, A[a]);
        PC += 1;
    }
}
