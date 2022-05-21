//
// Created by agwen on 13.05.22.
//


#include "common.h"

sem_t* oven_sem;
sem_t* table_sem;
sem_t* full_oven_sem;
sem_t* full_table_sem;
sem_t* empty_table_sem;

int shm_oven_fd;
int shm_table_fd;

oven* ovn;
table* tab;

void prep(int type) {
    printf("[CHEF]  (pid: %d time: %s) <*> "
           "Prepping pizza: %d\n",
           getpid(),
           get_current_time(),
           type);
    sleep(PREPARATION_TIME);

    // if full_oven_sem == 0 -> it blocks cook process
    // => decrement value before placing in oven
    lock_sem(full_oven_sem);
}

void place_n_bake(int type) {
    lock_sem(oven_sem);
    ovn->arr[ovn->to_place_idx] = type;
    ovn->to_place_idx++;
    ovn->to_place_idx = ovn->to_place_idx % SIZE_OVEN;
    ovn->pizzas++;
    printf("[CHEF]  (pid: %d time: %s) <*> "
           "Put in pizza: %d."
           " Pizzas in the oven: %d.\n",
           getpid(),
           get_current_time(),
           type,
           ovn->pizzas);
    unlock_sem(oven_sem);
    sleep(BAKING_TIME);
}

void take_out(int type) {
    lock_sem(oven_sem);
    type = ovn->arr[ovn->to_take_out_idx];
    ovn->arr[ovn->to_take_out_idx] = -1;
    ovn->to_take_out_idx++;
    ovn->to_take_out_idx = ovn->to_take_out_idx % SIZE_OVEN;
    ovn->pizzas--;
    printf("[CHEF]  (pid: %d time: %s) <*> "
           "Took out pizza: %d."
           " Pizzas in the oven: %d."
           " Pizzas on the table: %d.\n",
           getpid(),
           get_current_time(),
           type,
           ovn->pizzas,
           tab->pizzas);
    unlock_sem(oven_sem);

    // increment sema_oven_full cuz pizza out
    unlock_sem(full_oven_sem);

    // if sema_table_full == 0 <->  blocks cook process
    // => decrement value before putting on the tablee
    lock_sem(full_table_sem);
}

void on_table(int type) {
    lock_sem(table_sem);
    tab->arr[tab->to_place_idx] = type;
    tab->to_place_idx++;
    tab->to_place_idx = tab->to_place_idx % SIZE_OVEN;
    tab->pizzas++;
    printf("[CHEF]  (pid: %d time: %s) <*> Putting on the table pizza: %d. "
           "Pizzas left in the oven: %d. "
           "Pizzas on the table: %d.\n",
           getpid(),
           get_current_time(),
           type,
           ovn->pizzas,
           tab->pizzas);
    unlock_sem(table_sem);
}

int main(){
    oven_sem = get_sem(SEMA_OVEN);
    table_sem = get_sem(SEMA_TABLE);
    full_oven_sem = get_sem(SEMA_OVEN_FULL);
    full_table_sem = get_sem(SEMA_TABLE_FULL);
    empty_table_sem = get_sem(SEMA_TABLE_EMPTY);
    shm_oven_fd = get_shm_oven_fd();
    shm_table_fd = get_shm_table_fd();
    ovn = mmap(NULL, sizeof(oven), PROT_READ | PROT_WRITE, MAP_SHARED, shm_oven_fd, 0);
    tab = mmap(NULL, sizeof(table), PROT_READ | PROT_WRITE, MAP_SHARED, shm_table_fd, 0);

    int type;
    srand(getpid());

    while(1){
        type = rand() % 10;
        // preparing
        prep(type);

        // placing in oven
        // baking
        place_n_bake(type);

        // taking out
        take_out(type);

        // placing on the table
        on_table(type);

        // if empty_table_sem == 0 <-> blocks supplier process
        // => pizza was placed on the table -> increment its value
        // pizza can be taken by supplier
        unlock_sem(empty_table_sem);

    }

}