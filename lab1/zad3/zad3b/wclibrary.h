
#ifndef LAB1_EX01_H
#define LAB1_EX01_H
#include <stdio.h>

struct block {
    char* arr;
    long size;
};
int find_empty_index();
long get_file_size(FILE *fd);
void wc_command(char** files, int number_of_file);
void create_block_table(int number_of_blocks);
void remove_block(int index);
int reserve_block();
void print_block(int index);
void free_table();
#endif //LAB1_EX01_H
