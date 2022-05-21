//
// Created by agwen on 12.05.22.
//


#include "common.h"

int shm_oven_id, shm_table_id, sem_id;

void create_shared_memory_segment(){
    key_t key_o, key_t;
    TRY((key_o = ftok(OVEN_PATH, ID_OVEN)), "Cannot generate key [oven]!\n");
    TRY((key_t = ftok(TABLE_PATH, ID_TABLE)), "Cannot generate key [table]!\n");
    printf("[INFO] oven key: %d, table key: %d\n", key_o, key_t);
    TRY((shm_oven_id = shmget(key_o, sizeof(oven), IPC_CREAT | 0666)), "Cannot create shared memory segment [oven]!\n");
    TRY((shm_table_id = shmget(key_t, sizeof(table), IPC_CREAT | 0666)), "Cannot create shared memory segment [table]!\n");

    oven* ovn = shmat(shm_oven_id, NULL, 0);
    table* tab = shmat(shm_table_id, NULL, 0);

    for (int i = 0; i < SIZE_OVEN; i++){
        ovn->arr[i] = -1;
    }
    ovn->pizzas = 0;
    ovn->to_place_idx = 0;
    ovn->to_take_out_idx = 0;


    for (int i = 0; i < SIZE_OVEN; i++){
        tab->arr[i] = -1;
    }
    tab->pizzas = 0;
    tab->to_place_idx = 0;
    tab->to_take_out_idx = 0;

    printf("[INFO] OK, shared memory segment created\n[INFO] oven_id: %d, table_id: %d \n\n", shm_oven_id, shm_table_id);

}

void create_semaphores(){
    key_t key;
    TRY((key = ftok(get_home_path(), ID)), "Cannot generate key!\n");
    TRY((sem_id = semget(key, 5, 0666 | IPC_CREAT)), "Cannot create semaphore set!\n");
    union arg;
    arg.val = 1;

    TRY(semctl(sem_id, SEMA_OVEN, SETVAL, arg), "Cannot set oven semaphore value!\n");
    TRY(semctl(sem_id, SEMA_TABLE, SETVAL,arg), "Cannot set table semaphore value!\n");
    // oven is full -> block cook process == cannot put in pizza
    arg.val = SIZE_OVEN;
    TRY(semctl(sem_id, SEMA_OVEN_FULL, SETVAL,arg), "Cannot set table semaphore value!\n");

    // table is full -> block cook process - cannot place pizza
    arg.val = SIZE_TABLE;
    TRY(semctl(sem_id, SEMA_TABLE_FULL, SETVAL,arg), "Cannot set table semaphore value!\n");

    // table is empty -> block cook process - cannot take out pizza
    arg.val = 0;
    TRY(semctl(sem_id, SEMA_TABLE_EMPTY, SETVAL,arg), "Cannot set table semaphore value!\n");
    printf("[INFO] OK, semaphore set created\n[INFO] sem_id: %d\n\n", sem_id);
}

void handler(int signum){
    semctl(sem_id, 0, IPC_RMID, NULL);
    shmctl(shm_oven_id, IPC_RMID, NULL);
    shmctl(shm_table_id, IPC_RMID, NULL);
}


int main(int argc, char* argv[]){

    if (argc != 3){
        printf("[ERROR] Invalid number of arguments!\n");
        return -1;
    }

    int chefs = atoi(argv[1]);
    int ubers = atoi(argv[2]);

    signal(SIGINT, handler);

    create_shared_memory_segment();
    create_semaphores();

    for (int i = 0; i < chefs; i++){
        pid_t pid = fork();
        if (pid == 0){
            execl("./chef", "./chef", NULL);
            printf("[HUGE ERROR] RETURN NOT EXPECTED\n");
        }
    }

    for (int i = 0; i < ubers; i++){
        pid_t pid = fork();
        if (pid == 0){
            execl("./uberEats", "./uberEats", NULL);
            printf("[HUGE ERROR] RETURN NOT EXPECTED\n");
        }
    }

    for(int i = 0; i < chefs + ubers; i++)
        wait(NULL);


    return 0;
}