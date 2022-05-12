//
// Created by agwen on 12.05.22.
//


#include "common.h"

int sem_id;
int shm_oven_id;
int shm_table_id;

oven* ovn;
table* tab;

void place_in_oven(oven* ovn, int type){
    ovn->arr[ovn->to_place_idx] = type;
    ovn->to_place_idx++;
    ovn->to_place_idx = ovn->to_place_idx % SIZE_OVEN;
    ovn->pizzas++;
}

int take_out_pizza(oven* ovn){
    int type = ovn->arr[ovn->to_take_out_idx];
    ovn->arr[ovn->to_take_out_idx] = -1;
    ovn->to_take_out_idx++;
    ovn->to_take_out_idx = ovn->to_take_out_idx % SIZE_OVEN;
    ovn->pizzas--;
    return type;
}


void place_on_table(table* tab, int type){
    tab->arr[tab->to_place_idx] = type;
    tab->to_place_idx++;
    tab->to_place_idx = tab->to_place_idx % SIZE_OVEN;
    tab->pizzas++;
}

void prep(int type) {
    printf("[CHEF]  (pid: %d timestamp: %s)  ->   Przygotowuje pizze: %d\n",
           getpid(),
           get_current_time(),
           type);
    sleep(PREP_TIME);

    // if full_oven_sem is 0, it blocks cook process
    // so decrement value before placing in oven
    lock_sem(sem_id, SEMA_OVEN_FULL);
}

void place_n_bake(int type) {
    lock_sem(sem_id, SEMA_OVEN);
    place_in_oven(ovn, type);
    printf("[CHEF]  (pid: %d timestamp: %s)  ->   Dodałem pizze: %d. Liczba pizz w piecu: %d.\n",
           getpid(),
           get_current_time(),
           type,
           ovn->pizzas);
    unlock_sem(sem_id, SEMA_OVEN);
    sleep(BAKE_TIME);
}


void take_out() {
    lock_sem(sem_id, SEMA_OVEN);
    int type = take_out_pizza(ovn);
    printf("[CHEF]  (pid: %d timestamp: %s)  ->   Wyjalem pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d.\n",
           getpid(),
           get_current_time(),
           type,
           ovn->pizzas,
           tab->pizzas);
    unlock_sem(sem_id, SEMA_OVEN);

    // pizza was taken out, so increment full_oven_sem
    unlock_sem(sem_id, SEMA_OVEN_FULL);

    // if full_table_sem is 0, it blocks cook process
    // so decrement value before placing on the table
    lock_sem(sem_id, SEMA_TABLE_FULL);
}

void on_table(int type) {
    lock_sem(sem_id, SEMA_TABLE);
    place_on_table(tab, type);
    printf("[CHEF]  (pid: %d timestamp: %s)  ->   Umieszczam pizze na stole: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d.\n",
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

        // if empty_table_sem is 0, it blocks supplier process
        // because pizza was placed on the table, let's increment its value - pizza can be taken by supplier
        unlock_sem(sem_id, SEMA_TABLE_EMPTY);

    }

}