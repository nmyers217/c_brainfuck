#pragma once

#include <stdint.h>


typedef int (*ReadFn)();
typedef int (*WriteFn)(int);


typedef struct _Program {
    char* instructions;
    size_t instruction_count;
    uint16_t pc; // The program counter

    uint8_t* memory;
    size_t memory_size;
    uint8_t* dp; // The data pointer

    ReadFn read_fn;
    WriteFn write_fn;
} Program;


/**
 * Initialize a program to it's default state
 */
void init_program(
    Program* program,
    char const* instructions,
    size_t count,
    size_t memory_size,
    ReadFn read_fn,
    WriteFn write_fn
);


/**
 * Initialize a program from a file
 */
void init_program_from_file(
    Program* program,
    char const* filepath,
    size_t memory_size,
    ReadFn read_fn,
    WriteFn write_fn
);


/**
 * Free a program's memory
 */
void free_program(Program* program);


/**
 * Advance a program to its next state by:
 * 1) Lookup the instruction using pc
 * 2) Execute the instruction, modify the memory or dp if needed
 * 3) Advance pc accordingly
 */
void next_state(Program* program);

