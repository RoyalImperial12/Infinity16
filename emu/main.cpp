#include "i16.h"

#include <initializer_list>
#include <thread>
#include <vector>
#include <fstream>
#include <chrono>

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
        default: { return 1; break; }
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
        printf("R1:   0x%.4hx\nR2:   0x%.4hx\nR3:   0x%.4hx\nR4:   0x%.4hx\nR5:   0x%.4hx\nR6:   0x%.4hx\nR7:   0x%.4hx\nR8:   0x%.4hx\nR9:   0x%.4hx\nR10:  0x%.4hx\nR11:  0x%.4hx\nR12:  0x%.4hx\n",
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
        i16::CPU.registers[i16::R12]);
        printf("IP:   0x%.6lx\nBP:   0x%.6lx\nSP:   0x%.4x\nFLAG: 0x%.4x\n",
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
        } else if (x == i16::IP || x == i16::BP) {
            continue;
        } else if (i16::CPU.registers[x] > UINT16_MAX) {
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
    std::vector<uint8_t> dataBuffer;
    std::ifstream binFile("..\\out.bin", std::ios::in | std::ios::binary);
    if (binFile.is_open()) {
        dataBuffer = std::vector<uint8_t>(std::istreambuf_iterator<char>(binFile), {});
        binFile.close();
        int x = 0;
        for (uint8_t byte : dataBuffer) {
            i16::Memory.writeByte(x, byte);
            x++;
        }
        dataBuffer.clear();
    } else {
        printf("Binary File does not exist, skipping!...\n");
        memoryInitialise();
    }

    auto now = std::chrono::steady_clock::now();

    while (1) {
        auto curTime = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(curTime - now).count() >= 1) {
            monitor();
            now = std::chrono::steady_clock::now();
        }
        std::jthread executeThread(execute);
    }
    return 0;
}