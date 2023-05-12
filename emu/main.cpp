#include "i16.h"

#include <initializer_list>
#include <thread>

i16::cpu i16::CPU;
i16::memory i16::Memory;

bool conditionCheck(uint8_t cond) {
    switch (cond) {
        case 0: { return 1; break; } // Unconditional
        case 1: 
        case 2: { return i16::CPU.registers[i16::FLAG] & 0x2 >> 1; break; } // EQ NE
        case 3:
        case 4: { return i16::CPU.registers[i16::FLAG] & 0x1; break; } // CS CC
        case 5:
        case 6: { return i16::CPU.registers[i16::FLAG] & 0x4 >> 2; break; } // SN SP
        case 7:
        case 8: { return i16::CPU.registers[i16::FLAG] & 0x8 >> 3; break; } // OS OC
        case 9: { return (i16::CPU.registers[i16::FLAG] & 0x1 && i16::CPU.registers[i16::FLAG] & 0x2 >> 1); break; } // UH
        case 10: { return (~i16::CPU.registers[i16::FLAG] & 0x1 && i16::CPU.registers[i16::FLAG] & 0x2 >> 1); break; } // UL
        case 11:
        case 12: { return (i16::CPU.registers[i16::FLAG] & 0x4 >> 2 == i16::CPU.registers[i16::FLAG] & 0x8 >> 3); break; } // SH SL
        case 13: { return (~i16::CPU.registers[i16::FLAG] & 0x2 >> 1 && (i16::CPU.registers[i16::FLAG] & 0x4 >> 2 == i16::CPU.registers[i16::FLAG] & 0x8 >> 3)); break; } // SG
        case 14: { return (i16::CPU.registers[i16::FLAG] & 0x2 >> 1 || (i16::CPU.registers[i16::FLAG] & 0x4 >> 2 != i16::CPU.registers[i16::FLAG] & 0x8 >> 3)); break; } // SU
    }
}

void memoryInitialise(std::initializer_list<uint8_t> data = { 0x0 }) {
    int x = 0;
    for (uint8_t byte : data) {
        i16::Memory.writeByte(x, byte);
        x++;
    }
}

void monitor() {
    system("clear");
    printf("R1: %#hx\nR2: %#hx\nR3: %#hx\nR4: %#hx\nR5: %#hx\nR6: %#hx\nR7: %#hx\nR8: %#hx\nR9: %#hx\nR10: %#hx\nR11: %#hx\nR12: %#hx\nIP: %#x\nBP: %#x\nSP: %#hx\nFLAGS: %#hx", 
    i16::CPU.registers[i16::R1],
    i16::CPU.registers[i16::R2],
    i16::CPU.registers[i16::R3],
    i16::CPU.registers[i16::R4],
    i16::CPU.registers[i16::R5],
    i16::CPU.registers[i16::R6],
    i16::CPU.registers[i16::R7],
    i16::CPU.registers[i16::R8],
    i16::CPU.registers[i16::R9],
    i16::CPU.registers[i16::R10],
    i16::CPU.registers[i16::R11],
    i16::CPU.registers[i16::R12],
    i16::CPU.registers[i16::IP],
    i16::CPU.registers[i16::BP],
    i16::CPU.registers[i16::SP],
    i16::CPU.registers[i16::FLAG]);
} 

void execute() {
    for (int x = 0; x < i16::CPU.registers.size(); x++) {
        if ((x == i16::IP || x == i16::BP) && i16::CPU.registers[x] > MAX_ADDRESSABLE_MEMORY) {
            i16::CPU.registers[x] = 0x0;
            continue;
        }
        if (i16::CPU.registers[x] > UINT16_MAX) {
            i16::CPU.registers[x] = 0x0;
            continue;
        }
    }

    if (i16::CPU.interruptActive) {
        for (int x = 0; x < i16::CPU.interrupts.size(); x++) {
            if (i16::CPU.interrupts[x]) {
                uint32_t vector = x * 20 + 64;
                i16::sendData(1, i16::STACK, 0, (i16::CPU.registers[i16::IP] & 0xffff00) >> 8);
                i16::sendData(0, i16::STACK, 0, i16::CPU.registers[i16::IP] & 0xff);
                i16::CPU.registers[i16::IP] = vector;
            }
        }
    }
    uint16_t op1 = i16::Memory.readWord(i16::CPU.registers[i16::IP]);
    i16::CPU.registers[i16::IP] += 2;
    if (conditionCheck(OP1_COND)) {
        switch (OP1_OPG) {
            case 0x0: {
                i16::SYSexecute(op1);
                break;
            }
            case 0x1: {
                i16::DATexecute(op1);
                break;
            }
            case 0x2: {
                i16::ATHexecute(op1);
                break;
            }
            default: {
                break;
            }
        }
    }
}

int main() {
    memoryInitialise();
    std::thread monitorThread(monitor);
    std::thread executeThread(execute);
    
    monitorThread.detach();
    executeThread.join();

    monitorThread.~thread();
    return 0;
}
