//
// Created by agwen on 12.05.22.
//


#include "common.h"

int sem_id;
int shm_oven_id;
int shm_table_id;

oven* ovn;
table* tab;

void prep(int type) {
    printf("[CHEF]  (pid: %d time: %s) <*> "
           "Prepping pizza: %d\n",
           getpid(),
           get_current_time(),
           type);
    sleep(PREP_TIME);

    // if full_oven_sem == 0 -> it blocks cook process
    // => decrement value before placing in oven
    lock_sem(sem_id, SEMA_OVEN_FULL);
}

void place_n_bake(int type) {
    lock_sem(sem_id, SEMA_OVEN);
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
    unlock_sem(sem_id, SEMA_OVEN);
    sleep(BAKE_TIME);
}


void take_out() {
    lock_sem(sem_id, SEMA_OVEN);
    int type = ovn->arr[ovn->to_take_out_idx];
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
    unlock_sem(sem_id, SEMA_OVEN);

    // increment sema_oven_full cuz pizza out
    unlock_sem(sem_id, SEMA_OVEN_FULL);

    // if sema_table_full == 0 <->  blocks cook process
    // => decrement value before putting on the table
    lock_sem(sem_id, SEMA_TABLE_FULL);
}

void on_table(int type) {
    lock_sem(sem_id, SEMA_TABLE);
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
    unlock_sem(sem_id, SEMA_TABLE);
}

int main(){
    sem_id = get_sem_id();
    shm_oven_id = get_shm_oven_id();
    shm_table_id = get_shm_table_id();
    ovn = shmat(shm_oven_id, NULL, 0);
    tab = shmat(shm_table_id, NULL, 0);
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
        take_out();

        // placing on the table
        on_table(type);

        // if empty_table_sem == 0 <-> blocks supplier process
        // => pizza was placed on the table -> increment its value
        // pizza can be taken by supplier
        unlock_sem(sem_id, SEMA_TABLE_EMPTY);

    }

}