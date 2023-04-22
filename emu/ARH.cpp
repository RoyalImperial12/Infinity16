#include "i16.h"

// Instructions



// Executions

void i16::ATHexecute(uint16_t op1) {
    uint16_t op2 = i16::Memory.readWord(i16::CPU.registers[i16::IP]); i16::CPU.registers[i16::IP] += 2;
}