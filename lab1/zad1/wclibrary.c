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
        fprintf(stderr, "block size is incorrect\n");
    }

    blocks_table = calloc(number_of_blocks, sizeof(struct block));
    ARRAY_SIZE = number_of_blocks;
    printf("Allocated array of size: %d\n", number_of_blocks);
}

void count_stuff(char** files, int number_of_file) {
    if (number_of_file < 1) {
        printf("bo files no problems\n");
    }
    command_array[0] = '\0';
    strcat(command_array, "wc ");
    for (int i = 2; i < number_of_file + 2; ++i) {
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
    printf("index to: %d\n", index);
    struct block *single_block = calloc(1, sizeof(struct block));
    blocks_table[index] = single_block;
    single_block-> size = file_size;

    single_block->arr = calloc(file_size + 1, sizeof(char));

    if (single_block->arr == NULL) {
        printf("chuj nie da sie zaalokowac araya\n");
    }

    fread(single_block->arr, sizeof(char), file_size, tmp);
    printf("Allocated block of index [index], contents:\n %s\n", single_block->arr);
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
    printf("chuj dupa nie ma wolnych indexów\n");
    exit(-1);
}
void remove_block(int index) {
    if (blocks_table[index] == NULL) {
        printf("nie zwolnisz jak nie istnieje\n");
    }
    free(blocks_table[index]->arr);
    free(blocks_table[index]);
}

void print_block(int index) {
    printf("[INFO]\t block of index: %d\n"
           "contents: %s\n", index, blocks_table[index]->arr);
}

long get_file_size(FILE *fd) {
    fseek(fd, 0L, SEEK_END);
    int sz = ftell(fd);
    fseek(fd, 0L, SEEK_SET);
    return sz;
}
