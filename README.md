# Infinity16 - 16-Bit ISA
## About
The Infinity16 is an 16-Bit processor built upon my own made-up ISA, iISA (infinity Instruction Set Architecture). For more information about the instruction set and the architecture, please refer to the Wiki Section.
## Usage
As of current, the Assembler is extremely experimental as well as basic, lacking ASM comforts such as ORG, and only takes input during runtime, an 'end' input on a new line is required to end input. The assembler may not function as intended. When files are output from the Assembler, they will need to be in the root directory of the emulator for it to be read into memory. MemoryInitialise() is still available for manual memory insertion.

Please look at the Assembly page on the Wiki before creating an ASM program.
## Requirements
### Assembler
 - Python 3.11
## Build
As of current, the best means to build the emulator is via gcc (or Clang if you prefer). gcc method is as specified below:
```
cd emu
gcc *.cpp -o i16emu -lstdc++ -std=c++11
```
