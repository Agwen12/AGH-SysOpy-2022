//
// Created by agwen on 12.05.22.
//

#include "common.h"

int sem_id;
int shm_table_id;
int type;
table* tab;


void taking_out() {
    lock_sem(sem_id, SEMA_TABLE);
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
    unlock_sem(sem_id, SEMA_TABLE);
}


void deliver() {
    sleep(DELIVERY_TIME);
    printf("[UBER]  (pid: %d time: %s) <*> "
           "Delivering pizza: %d.\n",
           getpid(),
           get_current_time(),
           type);
    sleep(RETURN_TIME);
}
int main(){
    sem_id = get_sem_id();
    shm_table_id = get_shm_table_id();
    tab = shmat(shm_table_id, NULL, 0);
    while(1) {
        // if empty_table_sem == 0 -> blocks uber process
        // => decrement value before taking pizza out from table
        lock_sem(sem_id, SEMA_TABLE_EMPTY);
        taking_out();

        // if full_table_sem == 0 -> blocks cook process
        // because pizza was taken out of the table,
        // increment its value <-> pizza can be placed by cook
        unlock_sem(sem_id, SEMA_TABLE_FULL);
        deliver();
    }
}