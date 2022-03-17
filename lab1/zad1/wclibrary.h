//
// Created by agwen on 15.03.2022.
//

#ifndef LAB1_EX01_H
#define LAB1_EX01_H
#include <stdio.h>

struct block {
    char* arr;
    long size;
};
int find_empty_index();
long get_file_size(FILE *fd);
void count_stuff(char** files, int number_of_file);
void create_block_table(int number_of_blocks);
int third_function();
int reserve_block();
void print_block(int index);
#endif //LAB1_EX01_H
