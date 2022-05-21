//
// Created by agwen on 13.05.22.
//

#include "common.h"

sem_t* table_sem;
sem_t* full_table_sem;
sem_t* empty_table_sem;
int shm_table_fd;
table* tab;
int type;

void taking_out() {
    lock_sem(table_sem);
    type = tab->arr[tab->to_take_out_idx];
    tab->arr[tab->to_take_out_idx] = -1;
    tab->to_take_out_idx++;
    tab->to_take_out_idx = tab->to_take_out_idx % SIZE_TABLE;
    tab->pizzas--;
    printf("[UBER]  (pid: %d time: %s) <*> "
           "Taking Pizza: %d. "
           "Pizzas left on the table: %d.\n",
           getpid(),
           get_current_time(),
           type,
           tab->pizzas);
    unlock_sem(table_sem);
}

void delivery() {
    sleep(DELIVERY_TIME);
    printf("[UBER]  (pid: %d time: %s) <*> "
           "Delivering pizza: %d.\n",
           getpid(),
           get_current_time(),
           type);
    sleep(RETURN_TIME);
}

int main(){
    table_sem = get_sem(SEMA_TABLE);
    full_table_sem = get_sem(SEMA_TABLE_FULL);
    empty_table_sem = get_sem(SEMA_TABLE_EMPTY);
    shm_table_fd = get_shm_table_fd();
    tab = mmap(NULL, sizeof(table), PROT_READ | PROT_WRITE, MAP_SHARED, shm_table_fd, 0);
    while(1) {
        // if empty_table_sem == 0 -> blocks uber process
        // => decrement value before taking pizza out from table
        lock_sem(empty_table_sem);
        taking_out();

        // if full_table_sem == 0 -> blocks cook process
        // because pizza was taken out of the table,
        // increment its value <-> pizza can be placed by cook
        unlock_sem(full_table_sem);
        delivery();
    }
}