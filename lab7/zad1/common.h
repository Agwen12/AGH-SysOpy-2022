//
// Created by agwen on 12.05.22.
//

#ifndef LAB7_COMMON_H
#define LAB7_COMMON_H



#include <pwd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>
#include <sys//wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#define ID 420 //"R"
#define ID_OVEN 69 //"O"
#define ID_TABLE 2137 //"T"

#define TABLE_PATH "table"
#define OVEN_PATH "oven"

#define SIZE_TABLE 5
#define SIZE_OVEN 5

#define SEMA_TABLE_EMPTY 4
#define SEMA_TABLE_FULL 3
#define SEMA_OVEN_FULL 2
#define SEMA_TABLE 1
#define SEMA_OVEN 0


#define PREP_TIME 1
#define BAKE_TIME 4
#define RETURN_TIME 4
#define DELIVERY_TIME 4


#define BUFFER_SIZE 80

#define TRY(a, b) do { \
    if ((a) == -1) {   \
        printf(b);     \
        exit(1);       \
    }                  \
} while(0)


char* get_home_path() {
    char* path = getenv("HOME");
    if (path == NULL) {
        printf("[ERROR] Cannot get path!\n");
        exit(1);
        // moze wyjebywac program
    }
    return path;
}

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
} arg;

typedef struct{
    int arr[SIZE_OVEN];
    int pizzas;
    int to_place_idx;
    int to_take_out_idx;
} oven;

typedef struct{
    int arr[SIZE_TABLE];
    int pizzas;
    int to_place_idx;
    int to_take_out_idx;
} table;


int get_shm_table_id(){
    key_t key_t = ftok(TABLE_PATH, ID_TABLE);
    int shm_table_id;
    TRY((shm_table_id = shmget(key_t, sizeof(table), 0)),
        "[ERROR] Cannot get table ID!\n");
    return shm_table_id;
}

void lock_sem(int sem_id, int sem_num){
    // decrement sem
    struct sembuf s = {.sem_num = sem_num, .sem_op = -1};
    TRY(semop(sem_id, &s, 1),
        "[ERROR] Cannot lock semaphore\n");

}


int get_shm_oven_id(){
    key_t key_o = ftok(OVEN_PATH, ID_OVEN);
    int shm_oven_id;
    TRY((shm_oven_id = shmget(key_o, sizeof(oven), 0)),
        "[ERROR] Cannot get oven ID!\n");
    return shm_oven_id;
}

void unlock_sem(int sem_id, int sem_num) {
    // increment sem
    struct sembuf s = {.sem_num = sem_num, .sem_op = 1};
    TRY(semop(sem_id, &s, 1),
        "[ERROR] Cannot unlock semaphore\n");
}

int get_sem_id(){
    // connect to an already existing semaphore set
    key_t key = ftok(get_home_path(), ID);
    int sem_id;
    TRY((sem_id = semget(key, 5, 0)),
        "[ERROR] Cannot connect to semaphore set!\n");
    return sem_id;
}


char* get_current_time(){
    struct timeval cur_time;
    gettimeofday(&cur_time, NULL);
    int milli = cur_time.tv_usec / 1000;

    char* buffer = calloc(80, sizeof(char));
    strftime(buffer,
             BUFFER_SIZE,
             "%Y-%m-%d %H:%M:%S",
             localtime(&cur_time.tv_sec));

    char* current_time = calloc(BUFFER_SIZE + 5, sizeof(char));
    sprintf(current_time,
            "%s:%03d",
            buffer,
            milli);
    return current_time;
}


#endif //LAB7_COMMON_H
