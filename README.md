# Brainfuck Interpreter

This is just a quick and dirty Brainfuck Interpreter that I wrote in C. I can't promise it will even run on your system as I have only compiled and ran it on a 64bit arch linux system with GCC.

It is a safe interpreter that allows a user configured memory size. It prevents undefined behavior by ensuring the instruction and data pointers are never out of bounds. If so, the program terminates.

## Getting Started

### Prerequisites

You will need a C compiler like GCC.

### Installing
1) Clone the repository
2) Compile the bf.c file (I used GCC on linux), you can use the GNU make file to do this (if available on your machine) by running the make command

### Usage
The binary only accepts two command line arguments
1) *-f* filepath
2) *-m* memory_size (defaults to the standard of 30k)

### Example
```sh
./bf -f ./res/hello_world.bf
```
