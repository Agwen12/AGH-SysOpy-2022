//
// Created by agwen on 31.03.2022.
//
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if(argc != 2) {
        printf("[ERROR] Wrong number of arguments\n");
        exit(1);
    }

    printf("Hello from parent process\n");
    for (int i = 0; i < atoi(argv[1]); i++) {
     pid_t pid = fork();
        if (pid == -1) {
            printf("[ERROR] something went wrong with forking\n");
        }
        if (pid == 0) {
            printf("Hello! from process: %d\n", getpid());
            break;
        }
    }


    return 0;
}
