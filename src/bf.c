#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "./bf.h"


void init_jump_index(Program* program) {
    for (uint32_t i = 0; i < program->instruction_count; i++) {
        char instruction = program->instructions[i];

        int32_t jump_pc = -1;
        uint32_t bracket_level = 0;

        switch(instruction) {
            case '[':
                for (int32_t j = i; j < program->instruction_count; j++) {
                    if (program->instructions[j] == '[') {
                        bracket_level++;
                    } else if (program->instructions[j] == ']') {
                        bracket_level--;
                        if (bracket_level == 0) {
                            jump_pc = j;
                            break;
                        }
                    }
                }
                break;
            case ']':
                for (int32_t j = i; j >= 0; j--) {
                    if (program->instructions[j] == ']') {
                        bracket_level++;
                    } else if (program->instructions[j] == '[') {
                        bracket_level--;
                        if (bracket_level == 0) {
                            jump_pc = j;
                            break;
                        }
                    }
                }
                break;
            default:
                jump_pc = i;
                break; } 
        if (jump_pc < 0) {
            fprintf(stderr, "Bracket at index %i has no matching bracket!\n", i);
            exit(EXIT_FAILURE);
        }

        program->jump_index[i] = jump_pc;
    }
}


void init_program(
    Program* program,
    char const* instructions,
    size_t count,
    size_t memory_size,
    ReadFn read_fn,
    WriteFn write_fn
) {
    program->instructions = malloc(count);
    if (program->instructions == NULL) {
        fprintf(stderr, "Could not provision memory for instructions!\n");
        exit(EXIT_FAILURE);
    }

    program->jump_index = malloc(count * sizeof(uint32_t));
    if (program->jump_index == NULL) {
        fprintf(stderr, "Could not provision jump_index memory!\n");
        exit(EXIT_FAILURE);
    }

    program->instruction_count = count;
    program->pc = 0;

    if (memcpy(program->instructions, instructions, count) == NULL) {
        fprintf(stderr, "Could not copy instructions!\n");
        exit(EXIT_FAILURE);
    }

    program->memory = calloc(memory_size, 1);
    if (program->memory == NULL) {
        fprintf(stderr, "Could not provision program memory!\n");
        exit(EXIT_FAILURE);
    }

    program->memory_size = memory_size;
    program->dp = program->memory;

    program->read_fn = read_fn;
    program->write_fn = write_fn;

    init_jump_index(program);
}


void init_program_from_file(
    Program* program,
    char const* filepath,
    size_t memory_size,
    ReadFn read_fn,
    WriteFn write_fn
) {
    FILE* fp = fopen(filepath, "r");
    if (fp == NULL) {
        fprintf(stderr, "Could not open file %s\n", filepath);
        exit(EXIT_FAILURE);
    }

    fseek(fp, 0L, SEEK_END);
    size_t fsize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    char* buffer = malloc(fsize);
    if (buffer == NULL) {
        fprintf(stderr, "Could not provision memory for file %s\n", filepath);
        exit(EXIT_FAILURE);
    }

    if (fread(buffer, fsize, 1, fp) < 1) {
        fprintf(stderr, "Did not read the entirety of file %s\n", filepath);
        exit(EXIT_FAILURE);
    }

    if (fclose(fp) != 0) {
        fprintf(stderr, "Could not close file %s\n", filepath);
        exit(EXIT_FAILURE);
    }

    init_program(program, buffer, fsize, memory_size, read_fn, write_fn);

    free(buffer);
    buffer = NULL;
}


void free_program(Program* program) {
    free(program->instructions);
    program->instructions = NULL;

    free(program->jump_index);
    program->jump_index = NULL;

    free(program->memory);
    program->memory = NULL;

    program->instruction_count = 0;
    program->pc = 0;
    program->memory_size = 0;
    program->dp = 0;
    program->read_fn = NULL;
    program->write_fn = NULL;
}


void next_state(Program* program) {
    if (program->pc < 0 || program->pc >= program->instruction_count) {
        fprintf(stderr, "Invalid prorgram counter!");
        exit(EXIT_FAILURE);
    }


    if (program->dp < program->memory || program->dp >= program-> memory + program->memory_size) {
        fprintf(stderr, "Invalid data pointer!");
        exit(EXIT_FAILURE);
    }

    char instruction = program->instructions[program->pc];
    switch (instruction) {
        case '>':
            ++program->dp;
            break;
        case '<':
            --program->dp;
            break;
        case '+':
            ++*program->dp;
            break;
        case '-':
            --*program->dp;
            break;
        case '.':
            program->write_fn(*program->dp);
            break;
        case ',':
            *program->dp = program->read_fn();
            break;
        case '[':
            if (*program->dp == 0) {
                program->pc = program->jump_index[program->pc];
            }
            break;
        case ']':
            if (*program->dp != 0) {
                program->pc = program->jump_index[program->pc];
            }
            break;
        default:
            break;
    }

    program->pc++;
}


int main(int argc, char* const argv[]) {
    char const* filepath = NULL;
    size_t memory_size = 30000;

    int32_t c;
    while ((c = getopt (argc, argv, "f:m:")) != -1) {
        switch (c)
        {
            case 'f':
                filepath = optarg;
                break;
            case 'm':
                memory_size = atoi(optarg);
                break;
            case '?':
                if (optopt == 'f') {
                    fprintf(stderr, "Option -%c requires a filepath as an argument.\n", optopt);
                } else if (isprint(optopt)) {
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                } else {
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                }
                return EXIT_FAILURE;
            default:
                exit(EXIT_FAILURE);
        }
    }


    printf("Loading file (-f) %s with %li bytes of memory (-m)\n\n", filepath, memory_size);


    Program p;
    init_program_from_file(&p, filepath, memory_size, getchar, putchar);

    while(p.pc < p.instruction_count) {
        next_state(&p);
    }

    free_program(&p);

    return EXIT_SUCCESS;
}

