//
// Created by agwen on 13.05.22.
//

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
int ret;


void *printInfinite(void* v) {
    int iter = 0;

    int i = (int) v;
    while (1) {
        printf("Hello thread %d here, iteration %d\n", i, iter);
        iter++;
        if (iter == 100) {
            printf("Bye BITCH ~ %d\n", i);
            pthread_exit(&ret);
        }
    }
}


int main(int argc, char** argv) {
//    pthread_t hello_world_thread;
    int n = atoi(argv[1]);
    pthread_t[n] t;
    for(int i = 0; i < n; i++) {
        t[i] = calloc(1, sizeof(pthread_t*));
    }
    for (int i = 0; i < n; ++i) {
        pthread_create(&t[i], NULL, printInfinite, (void *) i);
    }
    int iter = 0;
    while (1) {
        printf("Hello MAIN THREAD HERE, iteration %d\n", iter);
        iter++;
        sleep(1);
        if (iter == 15) {
            return 0;
        }
    }
    return 0;
}