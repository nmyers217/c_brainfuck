#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./bf.h"


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
    program->instruction_count = 0;
    program->pc = 0;
    free(program->memory);
    program->memory = NULL;
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
            if (*program->dp > 0) {
                break;
            }
            uint32_t open_bracket_count = 0;
            // TODO: index jump locations in advance of running the program
            for (uint32_t i = program->pc; i < program->instruction_count; i++) {
                if (program->instructions[i] == '[') {
                    open_bracket_count++;
                } else if (program->instructions[i] == ']') {
                    open_bracket_count--;
                    if (open_bracket_count == 0) {
                        program->pc = i;
                        break;
                    }
                }
            }
            // TODO: error handling for no matching bracket
            break;
        case ']':
            if (*program->dp == 0) {
                break;
            }
            uint32_t close_bracket_count = 0;
            // TODO: index jump locations in advance of running the program
            for (uint32_t i = program->pc; i >= 0; i--) {
                if (program->instructions[i] == ']') {
                    close_bracket_count++;
                } else if (program->instructions[i] == '[') {
                    close_bracket_count--;
                    if (close_bracket_count == 0) {
                        program->pc = i;
                        break;
                    }
                }
            }
            // TODO: error handling for no matching bracket in advance of running the program
            break;
        default:
            break;
    }

    program->pc++;
}


int main(int argc, char const* argv[]) {
    // TODO: Error handling with args
    printf("%s %s %s\n", argv[0], argv[1], argv[2]);

    Program p;
    init_program_from_file(&p, argv[1], atoi(argv[2]), getchar, putchar);

    while(p.pc < p.instruction_count) {
        next_state(&p);
    }

    free_program(&p);

    return EXIT_SUCCESS;
}

