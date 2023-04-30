# Infinity16 - 16-Bit ISA
## About
The Infinity16 is an 16-Bit processor built upon my own made-up ISA, iISA (infinity Instruction Set Architecture). For more information about the instruction set and the architecture, please refer to the Wiki Section.
## Usage
As of current, the Assembler is still a WIP, however, data can still be inserted into the memory via the main's dataInitialise() function, but this requires recompilation each time this is done.
## Build
As of current, the best means to build the emulator is via gcc (or Clang if you prefer). gcc method is as specified below:
```
cd emu
gcc *.cpp -o i16emu -lstdc++ -std=c++11
```
