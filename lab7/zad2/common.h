//
// Created by agwen on 13.05.22.
//

#ifndef LAB7_COMMON_H
#define LAB7_COMMON_H


#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <stdbool.h>


#define SIZE_TABLE 5
#define SIZE_OVEN 5


#define SEMA_OVEN "/OVEN_S"
#define SEMA_TABLE "/TABLE_S"
#define SEMA_OVEN_FULL "/FULL_OVEN_S"
#define SEMA_TABLE_FULL "/FULL_TABLE_S"
#define SEMA_TABLE_EMPTY "/EMPTY_TABLE_S"

#define OVEN_SHM "/shm_oven"
#define TABLE_SHM "/shm_table"

#define PREPARATION_TIME 1
#define BAKING_TIME 4
#define DELIVERY_TIME 4
#define RETURN_TIME 4

#define BUFFER_SIZE 80


#define TRY_VAL(a, c, b) do {        \
    if ((a) == c) {                  \
        printf("[ERROR] %s", b);     \
        exit(1);                     \
    }                                \
} while(0)


#define TRY(a, b) do {               \
    if ((a) == -1) {                 \
        printf("[ERROR] %s", b);     \
        exit(1);                     \
    }                                \
} while(0)

typedef struct{
    int pizzas;
    int to_place_idx;
    int to_take_out_idx;
    int arr[];
} oven;

typedef struct{
    int pizzas;
    int to_place_idx;
    int to_take_out_idx;
    int arr[];
} table;


void lock_sem(sem_t* sem){
    // decrement sem
    TRY(sem_wait(sem), "Cannot lock semaphore\n");
}

void unlock_sem(sem_t* sem) {
    // increment sem
    TRY(sem_post(sem), "Cannot unlock semaphore\n");
}

sem_t* get_sem(char* name){
    // connect to an already existing semaphore
    sem_t* sem;
    if((sem = sem_open(name, O_RDWR)) == SEM_FAILED){
        printf("[ERROR] Cannot connect to semaphore  common[84]%s!\n", name);
        exit(1);
    }
    return sem;
}

int get_shm_oven_fd(){
    int shm_oven_fd;
    TRY((shm_oven_fd = shm_open(OVEN_SHM, O_RDWR , 0666)),
        "Cannot get oven id!\n");
    return shm_oven_fd;
}


int get_shm_table_fd(){
    int shm_table_fd;
    TRY((shm_table_fd = shm_open(TABLE_SHM, O_RDWR , 0666)),
        "Cannot get table id!\n");
    return shm_table_fd;
}

char* get_current_time(){
    struct timeval cur_time;
    gettimeofday(&cur_time, NULL);
    int milli = cur_time.tv_usec / 1000;

    char* buffer = calloc(80, sizeof(char));
    strftime(buffer,
             BUFFER_SIZE,
             "%H:%M:%S",
             localtime(&cur_time.tv_sec));

    char* current_time = calloc(BUFFER_SIZE + 5, sizeof(char));
    sprintf(current_time,
            "%s:%03d",
            buffer,
            milli);
    return current_time;
}



#endif //LAB7_COMMON_H
