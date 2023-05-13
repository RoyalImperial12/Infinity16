#include "i16.h"

// Defines

#define OP2_SRC (op2 & 0xf)
#define OP2_HL (op2 & 0x10 >> 4)
#define OP2_DST (op2 & 0x1e0 >> 5)
#define OP2_HL2 (op2 & 0x200 >> 9)

// Instructions

void STR(bool word, bool imme, uint8_t src, bool hl) {
    uint16_t data;
    if (imme) {
        data = i16::getData(word, i16::IMMEDIATE, 0);
    } else {
        data = i16::getData(word, hl ? i16::REGISTER_L : i16::REGISTER_H, src);
    }
    unsigned int addr = i16::Memory.readWord(i16::CPU.registers[i16::IP]) << 16 | i16::Memory.readByte(i16::CPU.registers[i16::IP] + 2); i16::CPU.registers[i16::IP] += 3;
    i16::sendData(word, i16::MEMORY, addr, data);
}

void LDR(bool word, bool imme, uint8_t dst, bool hl) {
    unsigned int addr;
    uint16_t data;
    if (imme) {
        data = i16::getData(word, i16::IMMEDIATE, 0);
    } else {
        addr = i16::Memory.readWord(i16::CPU.registers[i16::IP]) << 16 | i16::Memory.readByte(i16::CPU.registers[i16::IP] + 2); i16::CPU.registers[i16::IP] += 3;
        data = i16::getData(word, i16::MEMORY, addr);
    }
    i16::sendData(word, hl ? i16::REGISTER_L : i16::REGISTER_H, dst, data);
}

void MOV(bool word, bool imme, uint8_t src, bool hl, uint8_t dst, bool hl2) {
    uint16_t data;
    if (imme) {
        data = i16::getData(word, i16::IMMEDIATE, 0);
    } else {
        data = i16::getData(word, hl ? i16::REGISTER_L : i16::REGISTER_H, src);
    }
    i16::sendData(word, hl ? i16::REGISTER_L : i16::REGISTER_H, dst, data);
}

void PUSH(bool word, bool imme, uint8_t src, bool hl) {
    uint16_t data;
    if (imme) {
        data = i16::getData(word, i16::IMMEDIATE, 0);
    } else {
        data = i16::getData(word, hl ? i16::REGISTER_L : i16::REGISTER_H, src);
    }

    i16::sendData(word, i16::STACK, 0, data);
}

void POP(bool word, uint8_t dst, bool hl) {
    uint16_t data = i16::getData(word, i16::STACK, 0);
    i16::sendData(word, hl ? i16::REGISTER_L : i16::REGISTER_H, dst, data);
}

void SWP(bool word, uint8_t src, bool hl, uint8_t dst, bool hl2) {
    uint16_t sdata = i16::getData(word, hl ? i16::REGISTER_L : i16::REGISTER_H, src);
    uint16_t ddata = i16::getData(word, hl2 ? i16::REGISTER_L : i16::REGISTER_H, dst);

    i16::sendData(word, hl ? i16::REGISTER_L : i16::REGISTER_H, src, ddata);
    i16::sendData(word, hl2 ? i16::REGISTER_L : i16::REGISTER_H, dst, sdata);
}

// Execution

void i16::DATexecute(uint16_t op1) {
    uint16_t op2 = i16::Memory.readWord(i16::CPU.registers[i16::IP]); i16::CPU.registers[i16::IP] += 2;
    switch (OP1_OPCODE) {
        case 0x0: {
            STR(OP1_WORD, OP1_IMME, OP2_SRC, OP2_HL);
            break;
        }
        case 0x1: {
            LDR(OP1_WORD, OP1_IMME, OP2_DST, OP2_HL);
            break;
        }
        case 0x2: {
            MOV(OP1_WORD, OP1_IMME, OP2_SRC, OP2_HL, OP2_DST, OP2_HL2);
            break;
        }
        case 0x3: {
            PUSH(OP1_WORD, OP1_IMME, OP2_SRC, OP2_HL);
            break;
        }
        case 0x4: {
            POP(OP1_WORD, OP2_DST, OP2_HL2);
            break;
        }
        case 0x5: {
            SWP(OP1_WORD, OP2_SRC, OP2_HL, OP2_DST, OP2_HL2);
            break;
        }
        default: {
            break;
        }
    }
    return;
}