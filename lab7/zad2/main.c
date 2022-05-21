//
// Created by agwen on 13.05.22.
//


#include "common.h"


void create_shared_memory_segment(){
    int shm_oven_fd,shm_table_fd;
    TRY((shm_oven_fd = shm_open(OVEN_SHM, O_RDWR | O_CREAT, 0666)), "Cannot creat shared memory segment [oven]!\n");
    TRY((shm_table_fd = shm_open(TABLE_SHM, O_RDWR | O_CREAT, 0666)), "Cannot creat shared memory segment [table]!\n");
    // set shm segment size
    TRY(ftruncate(shm_oven_fd, sizeof(oven)), "ftruncate() not working [oven]\n");
    TRY(ftruncate(shm_table_fd, sizeof(table)), "ftruncate() not working [table]\n");
    // get address of attached segment
    oven* ovn = mmap(NULL, sizeof(oven), PROT_READ | PROT_WRITE, MAP_SHARED, shm_oven_fd, 0);
    table* tab = mmap(NULL, sizeof(table), PROT_READ | PROT_WRITE, MAP_SHARED, shm_table_fd, 0);
    TRY_VAL(ovn == (void*)-1, 1, "Cannot attach shm segment [oven]\n");
    TRY_VAL(tab == (void*)-1, 1, "Cannot attach shm segment [table]\n");

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

    printf("[INFO] OK, shared memory segment created.\n[INFO] oven_fd: %d, table_fd: %d\n", shm_oven_fd, shm_table_fd);

}

void create_semaphores(){
    TRY_VAL((sem_open(SEMA_OVEN, O_CREAT, 0666, 1)), SEM_FAILED, "Cannot create semaphore [OVEN]\n");
    TRY_VAL((sem_open(SEMA_TABLE, O_CREAT, 0666, 1)), SEM_FAILED, "Cannot create semaphore [TABLE]\n");
    // oven is full -> block cook process == cannot put in pizza
    TRY_VAL((sem_open(SEMA_OVEN_FULL, O_CREAT, 0666, SIZE_OVEN)), SEM_FAILED, "Cannot create semaphore [OVEN FULL]\n");
    // table is full -> block cook process - cannot place pizza
    TRY_VAL((sem_open(SEMA_TABLE_FULL, O_CREAT, 0666, SIZE_TABLE)), SEM_FAILED, "Cannot create semaphore [TABLE FULL]\n");
    // table is empty -> block cook process - cannot take out pizza
    TRY_VAL((sem_open(SEMA_TABLE_EMPTY, O_CREAT, 0666, 0)), SEM_FAILED, "Cannot create semaphore [TABLE EMPTY]\n");
    printf("[INFO] OK, semaphores created.\n\n");
}

void handler(int signum){
    sem_unlink(SEMA_OVEN);
    sem_unlink(SEMA_TABLE);
    sem_unlink(SEMA_OVEN_FULL);
    sem_unlink(SEMA_TABLE_FULL);
    sem_unlink(SEMA_TABLE_EMPTY);
    shm_unlink(TABLE_SHM);
    shm_unlink(OVEN_SHM);
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
