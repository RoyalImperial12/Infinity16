#include <inttypes.h>
#include <array>
#include <bitset>

// Defines

#define MAX_ADDRESSABLE_MEMORY 16777216

#define OP1_OPG (op1 & 0x7)
#define OP1_OPCODE (op1 & 0x78 >> 3)
#define OP1_COND (op1 & 0x780 >> 7)
#define OP1_IMME (op1 & 0x1000 >> 12)
#define OP1_CARRY (op1 & 0x2000 >> 13)
#define OP1_WORD (op1 & 0x8000 >> 15)

// Namespace Definition

namespace i16 {
    enum REGISTERS;
    enum MODES;

    class memory;
    class cpu;

    extern memory Memory;
    extern cpu CPU;

    extern void SYSexecute(uint16_t);
    extern void DATexecute(uint16_t);
    extern void ATHexecute(uint16_t);

    uint16_t getData(bool, uint8_t, unsigned int);
    void sendData(bool, uint8_t, unsigned int, uint16_t);
    void setFlags(bool, uint16_t, uint16_t = 0);
}

// Enumeration Definitions

enum i16::REGISTERS {
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R8,
    R9,
    R10,
    R11,
    R12,
    IP,
    BP,
    SP,
    FLAG
};

enum i16::MODES {
    REGISTER_H,
    REGISTER_L,
    MEMORY,
    IMMEDIATE,
    STACK
};

// Class Definitions

class i16::memory {
private:
    std::array<uint8_t, MAX_ADDRESSABLE_MEMORY> mem;
public:
    memory() {
        mem.fill(0);
    }

    uint8_t readByte(unsigned int);
    uint16_t readWord(unsigned int);

    void writeByte(unsigned int, uint8_t);
    void writeWord(unsigned int, uint16_t);
};

class i16::cpu {
private:
    
public:
    cpu() {
        interrupts.fill(false);
        interruptActive = false;
        registers.fill(0);
        registers[FLAG] == 0x8000;
    }

    std::array<bool, UINT16_MAX + 1> interrupts;
    bool interruptActive;

    std::array<uint16_t, UINT8_MAX + 1> ports;

    std::array<uint32_t, 16> registers;
};

// Inline Definitions

inline uint8_t i16::memory::readByte(unsigned int addr) {
    return mem[addr];
}
inline uint16_t i16::memory::readWord(unsigned int addr) {
    return mem[addr + 1] << 8 | mem[addr];
}
inline void i16::memory::writeByte(unsigned int addr, uint8_t data) {
    mem[addr] = data;
}
inline void i16::memory::writeWord(unsigned int addr, uint16_t data) {
    mem[addr + 1] = ((data & 0xff00) >> 8);
    mem[addr] = (data & 0xff);
}

// Helper Functions

// This is for ease of iteration, as you don't need to go around each of the individual functions to make a change.

uint16_t inline i16::getData(bool word, uint8_t mode, unsigned int addr) { // Retrieve Data
    if (word) {
        switch (mode) {
            case REGISTER_H: case REGISTER_L: {
                return i16::CPU.registers[addr];
                break;
            }
            case MEMORY: {
                return i16::Memory.readWord(addr);
                break;
            }
            case IMMEDIATE: {
                uint16_t data = i16::Memory.readWord(i16::CPU.registers[i16::IP]);
                i16::CPU.registers[i16::IP] += 2;
                return data;
                break;
            }
            case STACK: {
                uint16_t data = i16::Memory.readByte(i16::CPU.registers[i16::BP] + i16::CPU.registers[i16::SP]);
                data = i16::Memory.readByte(i16::CPU.registers[i16::BP] + i16::CPU.registers[i16::SP] - 1) << 8;
                i16::CPU.registers[i16::SP] -= 2;
                return data;
                break;
            }
        }
    } else {
        switch (mode) {
            case REGISTER_H: {
                return i16::CPU.registers[addr] & 0xff00 >> 8;
                break;
            }
            case REGISTER_L: {
                return i16::CPU.registers[addr] & 0xff;
                break;
            }
            case MEMORY: {
                return i16::Memory.readByte(addr);
                break;
            }
            case IMMEDIATE: {
                uint8_t data = i16::Memory.readByte(i16::CPU.registers[i16::IP]);
                i16::CPU.registers[i16::IP]++;
                return data;
                break;
            }
            case STACK: {
                uint8_t data = i16::Memory.readByte(i16::CPU.registers[i16::BP] + i16::CPU.registers[i16::SP]);
                i16::CPU.registers[i16::SP]--;
                return data;
                break;
            }
        }
    }
}

void inline i16::sendData(bool word, uint8_t mode, unsigned int addr, uint16_t data) { // Send Data
    if (word) {
        switch (mode) {
            case REGISTER_H: case REGISTER_L: {
                i16::CPU.registers[addr] = data;
                break;
            }
            case MEMORY: {
                i16::Memory.writeWord(addr, data);
                break;
            }
            case STACK: {
                i16::Memory.writeWord(i16::CPU.registers[i16::BP] + i16::CPU.registers[i16::SP], data);
                i16::CPU.registers[i16::SP] += 2;
                break;
            }
        }
    } else {
        switch (mode) {
            case REGISTER_H: {
                i16::CPU.registers[addr] = (i16::CPU.registers[addr] & 0xff) |  data << 8;
                break;
            }
            case REGISTER_L: {
                i16::CPU.registers[addr] = (i16::CPU.registers[addr] & 0xff00) |  data;
                break;
            }
            case MEMORY: {
                i16::Memory.writeByte(addr, data);
                break;
            }
            case STACK: {
                i16::Memory.writeByte(i16::CPU.registers[i16::BP] + i16::CPU.registers[i16::SP], data);
                i16::CPU.registers[i16::SP]++;
                break;
            }
        }
    }
}

void inline i16::setFlags(bool word, uint32_t data, uint16_t main) {
    bool msfData = word ? (data & 0x8000) >> 15 : (data & 0x80) >> 7;
    bool msfMain = word ? (main & 0x8000) >> 15 : (main & 0x80) >> 7;

    // Carry Bit
    if ((word && data > UINT16_MAX) || (!word && data > UINT8_MAX)) { // Set
        i16::CPU.registers[i16::FLAG] = (i16::CPU.registers[i16::FLAG] & 0xfffe) | word ? (data > UINT16_MAX) : (data > UINT8_MAX);
    } else { // Clear
        i16::CPU.registers[i16::FLAG] = (i16::CPU.registers[i16::FLAG] & 0xfffe);
    }

    // Zero Bit
    if (data == 0) { // Set
        i16::CPU.registers[i16::FLAG] = (i16::CPU.registers[i16::FLAG] & 0xfffd) | 2;
    } else { // Clear
        i16::CPU.registers[i16::FLAG] = (i16::CPU.registers[i16::FLAG] & 0xfffd);
    }

    // Sign Bit
    i16::CPU.registers[i16::FLAG] = (i16::CPU.registers[i16::FLAG] & 0xfffb) | msfData << 2; // Set or Clear

    // Overflow Bit
    if (msfData != msfMain) { // Set
        i16::CPU.registers[i16::FLAG] = (i16::CPU.registers[i16::FLAG] & 0xfff7) | 4;
    } else { // Clear
        i16::CPU.registers[i16::FLAG] = (i16::CPU.registers[i16::FLAG] & 0xfff7);
    }
}