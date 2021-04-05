#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


#include "suffix.h"

// 500 kB
#define BLOCK_SIZE 524288

int main(int argc, char* argv[]) {

    FILE* file = fdopen(dup(fileno(stdin)), "rb");
    if (file == NULL) {
        fputs("Error while reading from stdin\n", stderr);
    }

    size_t file_size = 0;
    size_t block_size = 0;
    
    uint8_t *read_block = malloc(sizeof(uint8_t) * BLOCK_SIZE);
    if (read_block == NULL) {
        fputs("Error: could not allocate memory for read_block\n", stderr);
        exit(-1);
    }

    while ((block_size = fread(&(read_block[file_size]), sizeof(uint8_t), BLOCK_SIZE, file)) != 0) {

        file_size += block_size;

        if (block_size < BLOCK_SIZE) {
            break;
        }

        // Grow read_block
        read_block = realloc(read_block, file_size + BLOCK_SIZE);
        if (read_block == NULL) {
            fputs("Error: could not allocate memory for read_block\n", stderr);
            exit(-1);
        }
    } 

    // shrink read_block to right size
    read_block = realloc(read_block, file_size);
    if (read_block == NULL) {
        fputs("Error: could not allocate memory for read_block\n", stderr);
        exit(-1);
    }

    build_and_print_suffix_tree(read_block, (int64_t)file_size);

    free(read_block);

    (void)argv;
    (void)argc;

    return 0;
}
