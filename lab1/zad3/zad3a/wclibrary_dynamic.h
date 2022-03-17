//
// Created by agwen on 17.03.2022.
//

#ifndef ZAD3_WCLIBRARY_DYNAMIC_H
#define ZAD3_WCLIBRARY_DYNAMIC_H

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

static void* handle = NULL;


void (*_create_block_table)(int);
void (*_wc_command)(char**, int);
void (*_remove_block)(int);
int (*_reserve_block)();
void (*_free_table)();
void free_handle();

void init_handle() {
    handle = dlopen("libwclibrary.so", RTLD_LAZY);
    if (handle == NULL) {
        printf("[ERROR] Could not load dynamic library\n");
        return;
    }
    printf("[INFO] Dynamic library loaded successfully\n");

    _create_block_table = dlsym(handle, "create_block_table");
    if (_create_block_table == NULL) {
        printf("[ERROR] create_block_table() function not loaded\n");
    }

    _wc_command = dlsym(handle, "wc_command");
    if (_wc_command == NULL) {
        printf("[ERROR] wc_command() function not loaded\n");
    }

    _remove_block = dlsym(handle, "remove_block");
    if (_remove_block == NULL) {
        printf("[ERROR] remove_block() function not loaded\n");
    }

    _reserve_block = dlsym(handle, "reserve_block");
    if (_reserve_block == NULL) {
        printf("[ERROR] reserve_block() function not loaded\n");
    }

    _free_table = dlsym(handle, "free_table");
    if (_reserve_block == NULL) {
        printf("[ERROR] free_table() function not loaded\n");
    }
}

void create_block_table(int number_of_blocks) {
    (*_create_block_table)(number_of_blocks);
}
void wc_command(char** files, int number_of_files){
    (*_wc_command)(files, number_of_files);
}
void remove_block(int index) {
    (*_remove_block)(index);
}

int reserve_block() {
    return (*_reserve_block)();
}

void free_table() {
    (*_free_table)();
}

void free_handle() {
    dlclose(handle);
}

#endif //ZAD3_WCLIBRARY_DYNAMIC_H
