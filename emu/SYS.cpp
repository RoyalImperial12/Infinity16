#include "i16.h"

// Instructions

void EI() {
    i16::CPU.registers[i16::FLAG] |= 0x8000;
}

void DI() {
    i16::CPU.registers[i16::FLAG] &= 0x7fff;
}

void INT() {
    uint16_t val = i16::Memory.readByte(i16::CPU.registers[i16::IP]); i16::CPU.registers[i16::IP] ++;
    i16::CPU.interrupts[val] = true;
    i16::CPU.interruptActive = true;
}

void IN(bool word) {
    uint16_t operands = i16::Memory.readWord(i16::CPU.registers[i16::IP]); i16::CPU.registers[i16::IP] += 2;
    uint16_t tmp = i16::CPU.ports[(operands & 0xff)];
    uint16_t data;
    if (word) {
        data = tmp;
        i16::CPU.registers[operands & 0xf000 >> 12] = data;
    } else {
        data = (operands & 0x800 >> 2) ? tmp << 8 : tmp & 0xff;
        i16::CPU.registers[operands & 0xf000 >> 12] = (i16::CPU.registers[operands & 0xf000 >> 12] & (operands & 0x800 >> 2) ? 0xff : 0xff00) | data;
    }
}

void OUT(bool word) {
    uint16_t operands = i16::Memory.readWord(i16::CPU.registers[i16::IP]); i16::CPU.registers[i16::IP] += 2;
    uint16_t tmp = i16::CPU.registers[operands & 0xf000 >> 12];
    uint16_t data;
    if (word) {
        data = tmp;
    } else {
        data = (operands & 0x800 >> 2) ? tmp & 0xff : tmp & 0xff00 >> 8;
    }
    i16::CPU.ports[(data & 0xff)] = data;
}

void JMP(bool imme) {
    uint32_t addr;
    if (imme) {
        addr = i16::getData(1, i16::IMMEDIATE, 0);
        i16::CPU.registers[i16::IP] = i16::CPU.registers[i16::IP] + addr;
    } else {
        addr = i16::getData(1, i16::IMMEDIATE, 0) << 8 | i16::getData(0, i16::IMMEDIATE, 0);
        i16::CPU.registers[i16::IP] = addr;
    }
}

void CLL(bool imme) {
    uint32_t addr;
    if (imme) {
        addr = i16::getData(1, i16::IMMEDIATE, 0);
        i16::sendData(1, i16::STACK, 0, (i16::CPU.registers[i16::IP] & 0xffff00) >> 8);
        i16::sendData(0, i16::STACK, 0, i16::CPU.registers[i16::IP] & 0xff);
        i16::CPU.registers[i16::IP] = i16::CPU.registers[i16::IP] + addr;
    } else {
        addr = i16::getData(1, i16::IMMEDIATE, 0) << 8 | i16::getData(0, i16::IMMEDIATE, 0);
        i16::sendData(1, i16::STACK, 0, (i16::CPU.registers[i16::IP] & 0xffff00) >> 8);
        i16::sendData(0, i16::STACK, 0, i16::CPU.registers[i16::IP] & 0xff);
        i16::CPU.registers[i16::IP] = addr;
    }
}

void RET() {
    uint32_t addr = i16::getData(1, i16::STACK, 0) << 8 | i16::getData(0, i16::STACK, 0);
    i16::CPU.registers[i16::IP] = addr;
}

void HLT() {
    exit(0);
}
void RST() {
    i16::CPU.registers[i16::IP] = 0x0;
}

// Execution

void i16::SYSexecute(uint16_t op1) {
    switch (OP1_OPCODE) {
        case 0x0: {
            break;
        }
        case 0x1: {
            EI();
            break;
        }
        case 0x2: {
            DI();
            break;
        }
        case 0x3: {
            INT();
            break;
        }
        case 0x4: {
            IN(OP1_WORD);
            break;
        }
        case 0x5: {
            OUT(OP1_WORD);
            break;
        }
        case 0x6: {
            JMP(OP1_IMME);
            break;
        }
        case 0x7: {
            CLL(OP1_IMME);
            break;
        }
        case 0x8: {
            RET();
            break;
        }
        case 0x9: {
            HLT();
            break;
        }
        case 0xa: {
            RST();
            break;
        }
        default: {
            break;
        }
    }
    return;
}