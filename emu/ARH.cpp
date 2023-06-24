#include "i16.h"

// Defines

#define OP2_MAIN (op2 & 0xf)
#define OP2_I2 (op2 & 0x10 >> 4)
#define OP2_SECOND (op2 & 0x1e0 >> 5)
#define OP2_DST (op2 & 0x1e00 >> 9)
#define OP2_HL (op2 & 0x2000 >> 13)
#define OP2_FLAG (op2 & 0x4000 >> 14)

// Instructions

void ADD(bool word, bool carry, bool flag, bool imme, uint8_t main, bool imme2, uint8_t second, uint8_t dst, bool hl) {
    uint16_t data1;
    uint16_t data2;

    if (imme) {
        data1 = i16::getData(word, i16::IMMEDIATE, 0);
    } else {
        data1 = i16::getData(word, i16::REGISTER_L, main);
    }

    if (imme2) {
        data2 = i16::getData(word, i16::IMMEDIATE, 0);
    } else {
        data2 = i16::getData(word, i16::REGISTER_L, second);
    }

    uint32_t res;

    if (carry) {
        res = data1 + data2 + (i16::CPU.registers[i16::FLAG] & 0x1);
    } else {
        res = data1 + data2;
    }

    if (flag) { i16::setFlags(word, res, data1); }

    i16::sendData(word, hl ? i16::REGISTER_L : i16::REGISTER_H, dst, res);
}

void SUB(bool word, bool carry, bool flag, bool imme, uint8_t main, bool imme2, uint8_t second, uint8_t dst, bool hl) {
    uint16_t data1;
    uint16_t data2;

    if (imme) {
        data1 = i16::getData(word, i16::IMMEDIATE, 0);
    } else {
        data1 = i16::getData(word, i16::REGISTER_L, main);
    }

    if (imme2) {
        data2 = i16::getData(word, i16::IMMEDIATE, 0);
    } else {
        data2 = i16::getData(word, i16::REGISTER_L, second);
    }

    uint32_t res;

    if (carry) {
        res = data1 - data2 + (i16::CPU.registers[i16::FLAG] & 0x1);
    } else {
        res = data1 - data2;
    }

    if (flag) { i16::setFlags(word, res, data1); }

    i16::sendData(word, hl ? i16::REGISTER_L : i16::REGISTER_H, dst, res);
}

void MUL(bool word, bool carry, bool flag, bool imme, uint8_t main, bool imme2, uint8_t second, uint8_t dst, bool hl) {
    uint16_t data1;
    uint16_t data2;

    if (imme) {
        data1 = i16::getData(word, i16::IMMEDIATE, 0);
    } else {
        data1 = i16::getData(word, i16::REGISTER_L, main);
    }

    if (imme2) {
        data2 = i16::getData(word, i16::IMMEDIATE, 0);
    } else {
        data2 = i16::getData(word, i16::REGISTER_L, second);
    }

    uint32_t res;

    if (carry) {
        res = data1 * data2 + (i16::CPU.registers[i16::FLAG] & 0x1);
    } else {
        res = data1 * data2;
    }

    if (flag) { i16::setFlags(word, res, data1); }

    i16::sendData(word, hl ? i16::REGISTER_L : i16::REGISTER_H, dst, res);
}

void DIV(bool word, bool carry, bool flag, bool imme, uint8_t main, bool imme2, uint8_t second, uint8_t dst, bool hl) {
    uint16_t data1;
    uint16_t data2;

    if (imme) {
        data1 = i16::getData(word, i16::IMMEDIATE, 0);
    } else {
        data1 = i16::getData(word, i16::REGISTER_L, main);
    }

    if (imme2) {
        data2 = i16::getData(word, i16::IMMEDIATE, 0);
    } else {
        data2 = i16::getData(word, i16::REGISTER_L, second);
    }

    uint32_t res;

    if (carry) {
        res = data1 / data2 + (i16::CPU.registers[i16::FLAG] & 0x1);
    } else {
        res = data1 / data2;
    }

    if (res == 0) {
        i16::CPU.interruptActive = true;
        i16::CPU.interrupts[0] = true;
    }

    if (flag) { i16::setFlags(word, res, data1); }

    i16::sendData(word, hl ? i16::REGISTER_L : i16::REGISTER_H, dst, res);
}

void AND(bool word, bool imme, uint8_t main, bool imme2, uint8_t second, uint8_t dst, bool hl) {
    uint16_t data1;
    uint16_t data2;

    if (imme) {
        data1 = i16::getData(word, i16::IMMEDIATE, 0);
    } else {
        data1 = i16::getData(word, i16::REGISTER_L, main);
    }

    if (imme2) {
        data2 = i16::getData(word, i16::IMMEDIATE, 0);
    } else {
        data2 = i16::getData(word, i16::REGISTER_L, second);
    }

    uint16_t res = data1 & data2;

    i16::sendData(word, hl ? i16::REGISTER_L : i16::REGISTER_H, dst, res);
}

void OR(bool word, bool imme, uint8_t main, bool imme2, uint8_t second, uint8_t dst, bool hl) {
    uint16_t data1;
    uint16_t data2;

    if (imme) {
        data1 = i16::getData(word, i16::IMMEDIATE, 0);
    } else {
        data1 = i16::getData(word, i16::REGISTER_L, main);
    }

    if (imme2) {
        data2 = i16::getData(word, i16::IMMEDIATE, 0);
    } else {
        data2 = i16::getData(word, i16::REGISTER_L, second);
    }

    uint16_t res = data1 | data2;

    i16::sendData(word, hl ? i16::REGISTER_L : i16::REGISTER_H, dst, res);
}

void XOR(bool word, bool imme, uint8_t main, bool imme2, uint8_t second, uint8_t dst, bool hl) {
    uint16_t data1;
    uint16_t data2;

    if (imme) {
        data1 = i16::getData(word, i16::IMMEDIATE, 0);
    } else {
        data1 = i16::getData(word, i16::REGISTER_L, main);
    }

    if (imme2) {
        data2 = i16::getData(word, i16::IMMEDIATE, 0);
    } else {
        data2 = i16::getData(word, i16::REGISTER_L, second);
    }

    uint16_t res = data1 ^ data2;

    i16::sendData(word, hl ? i16::REGISTER_L : i16::REGISTER_H, dst, res);
}

void NEG(bool word, bool imme, uint8_t main, uint8_t dst, bool hl) {
    uint16_t data;

    if (imme) {
        data = i16::getData(word, i16::IMMEDIATE, 0);
    } else {
        data = i16::getData(word, i16::REGISTER_L, main);
    }

    uint16_t res = ~data;

    i16::sendData(word, hl ? i16::REGISTER_L : i16::REGISTER_H, dst, res);
}

void RSL(bool word, uint8_t main, uint8_t dst, bool hl) {
    uint16_t data;

    data = i16::getData(word, i16::REGISTER_L, main);

    uint16_t temp1 = data << 1;
    uint16_t temp2 = word ? data >> 15 : data >> 7;
    uint16_t res = temp1 | temp2;

    i16::sendData(word, hl ? i16::REGISTER_L : i16::REGISTER_H, dst, res);
}

void RSR(bool word, uint8_t main, uint8_t dst, bool hl) {
    uint16_t data;

    data = i16::getData(word, i16::REGISTER_L, main);

    uint16_t temp1 = data >> 1;
    uint16_t temp2 = word ? data << 15 : data << 7;
    uint16_t res = temp1 | temp2;

    i16::sendData(word, hl ? i16::REGISTER_L : i16::REGISTER_H, dst, res);
}

void CMP(bool word, bool imme, uint8_t main, bool imme2, uint8_t second) {
    uint16_t data1;
    uint16_t data2;

    if (imme) {
        data1 = i16::getData(word, i16::IMMEDIATE, 0);
    } else {
        data1 = i16::getData(word, i16::REGISTER_L, main);
    }

    if (imme2) {
        data2 = i16::getData(word, i16::IMMEDIATE, 0);
    } else {
        data2 = i16::getData(word, i16::REGISTER_L, second);
    }

    uint32_t res;

    res = data1 - data2;

    i16::setFlags(word, res, data1);
}

void INC(bool word, uint8_t main) {
    uint16_t data = i16::getData(word, i16::REGISTER_L, main);
    data++;
    i16::sendData(data, i16::REGISTER_L, main, data);
}

void DCR(bool word, uint8_t main) {
    uint16_t data = i16::getData(word, i16::REGISTER_L, main);
    data--;
    i16::sendData(data, i16::REGISTER_L, main, data);
}

// Executions

void i16::ATHexecute(uint16_t op1) {
    uint16_t op2 = i16::Memory.readWord(i16::CPU.registers[i16::IP]); i16::CPU.registers[i16::IP] += 2;
    switch (OP1_OPCODE) {
        case 0x0: {
            ADD(OP1_WORD, OP1_CARRY, OP2_FLAG, OP1_IMME, OP2_MAIN, OP2_I2, OP2_SECOND, OP2_DST, OP2_HL);
            break;
        }
        case 0x1: {
            SUB(OP1_WORD, OP1_CARRY, OP2_FLAG, OP1_IMME, OP2_MAIN, OP2_I2, OP2_SECOND, OP2_DST, OP2_HL);
            break;
        }
        case 0x2: {
            MUL(OP1_WORD, OP1_CARRY, OP2_FLAG, OP1_IMME, OP2_MAIN, OP2_I2, OP2_SECOND, OP2_DST, OP2_HL);
            break;
        }
        case 0x3: {
            DIV(OP1_WORD, OP1_CARRY, OP2_FLAG, OP1_IMME, OP2_MAIN, OP2_I2, OP2_SECOND, OP2_DST, OP2_HL);
            break;
        }
        case 0x4: {
            AND(OP1_WORD, OP1_IMME, OP2_MAIN, OP2_I2, OP2_SECOND, OP2_DST, OP2_HL);
            break;
        }
        case 0x5: {
            OR(OP1_WORD, OP1_IMME, OP2_MAIN, OP2_I2, OP2_SECOND, OP2_DST, OP2_HL);
            break;
        }
        case 0x6: {
            XOR(OP1_WORD, OP1_IMME, OP2_MAIN, OP2_I2, OP2_SECOND, OP2_DST, OP2_HL);
            break;
        }
        case 0x7: {
            NEG(OP1_WORD, OP1_IMME, OP2_MAIN, OP2_DST, OP2_HL);
            break;
        }
        case 0x8: {
            RSL(OP1_WORD, OP2_MAIN, OP2_DST, OP2_HL);
            break;
        }
        case 0x9: {
            RSR(OP1_WORD, OP2_MAIN, OP2_DST, OP2_HL);
            break;
        }
        case 0xa: {
            CMP(OP1_WORD, OP1_IMME, OP2_MAIN, OP2_I2, OP2_SECOND);
            break;
        }
        case 0xb: {
            INC(OP1_WORD, OP2_MAIN);
            break;
        }
        case 0xc: {
            DCR(OP1_WORD, OP2_MAIN);
            break;
        }
        default: {
            break;
        }
    }
    return;
}