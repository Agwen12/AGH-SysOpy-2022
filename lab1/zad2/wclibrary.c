//
// Created by agwen on 15.03.2022.
//
#include <stdlib.h>
#include <stdio.h>

#include "string.h"
#include "wclibrary.h"

struct block** blocks_table;
char* temp_file_name;
int ARRAY_SIZE;
char command_array[1000];

void create_block_table(int number_of_blocks) {

    if (number_of_blocks < 1) {
        printf("[ERROR] Block size is incorrect\n");
        exit(-1);
    }

    blocks_table = calloc(number_of_blocks, sizeof(struct block));
    ARRAY_SIZE = number_of_blocks;
    printf("[INFO] Allocated array of size: %d\n", number_of_blocks);
}

void wc_command(char** files, int number_of_file) {
    if (number_of_file < 1) {
        printf("[ERROR] wc_command requires files as input\n");
        exit(-1);
    }
    command_array[0] = '\0';
    printf("[INFO] Performing wc_command on %d files\n", number_of_file);
    strcat(command_array, "wc ");
    for (int i = 0; i < number_of_file; ++i) {
        strcat(command_array, files[i]);
        strcat(command_array, " ");
    }

    strcat(command_array, "> temp");
    system(command_array);
}

int reserve_block() {
    FILE *tmp = fopen("temp", "r");
    long file_size = get_file_size(tmp);

    int index = find_empty_index();
    struct block *single_block = calloc(1, sizeof(struct block));
    blocks_table[index] = single_block;
    single_block-> size = file_size;

    single_block->arr = calloc(file_size + 1, sizeof(char));

    if (single_block->arr == NULL) {
        printf("[ERROR] Block cannot be allocated\n");
    }

    fread(single_block->arr, sizeof(char), file_size, tmp);
    printf("[INFO] Allocated block of index: %d\n", index);
    single_block->arr[file_size] = '\0';
    fclose(tmp);
    return index;
}

int find_empty_index() {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        if (blocks_table[i] == NULL) {
            return i;
        }
    }
    printf("[ERROR] Table is full\n");
    exit(-1);
}
void remove_block(int index) {
    if (blocks_table[index] == NULL) {
        printf("[ERROR] Block is already empty\n");
    }
    printf("[INFO] Removing block with index: %d\n", index);
    free(blocks_table[index]->arr);
    free(blocks_table[index]);
    blocks_table[index] = NULL;
}

void print_block(int index) {
    printf("[INFO] Block of index: %d\n", index);
}

long get_file_size(FILE *fd) {
    fseek(fd, 0L, SEEK_END);
    int sz = ftell(fd);
    fseek(fd, 0L, SEEK_SET);
    return sz;
}

void free_table() {
    printf("[INFO] Program has finished, freeing memory..\n");
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        if (blocks_table[i] != NULL) remove_block(i);
    }
    free(blocks_table);
}
