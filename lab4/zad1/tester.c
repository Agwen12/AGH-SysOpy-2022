//
// Created by agwen on 05.04.2022.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


int get_mode(char* command);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("[ERROR] Invalid number of arguments!\n");
        exit(-1);
    }

    switch (get_mode(argv[1])) {
        case 1:
            raise(SIGUSR1);
            printf("[INFO] Signal ignored (child)\n");
            break;
        case 2:
            printf("[ERROR] This is illegal!!!\n");
            break;
        case 3:
            raise(SIGUSR1);
            printf("[INFO] Signal masked x0 (child)\n");
            break;
        case 4:
            ;sigset_t pending_sigs;
            sigpending(&pending_sigs);
            printf("[INFO] Signal pending... (child) %s\n",
                   sigismember(&pending_sigs, SIGUSR1)?"YAS":"NAY");
            break;
        default:
            printf("[ERROR] Something went wrong!\n");
            exit(-1);
    }
    return 0;
}

int get_mode(char* command) {
    if (strcmp("ignore", command) == 0) {
        return 1;
    } else if (strcmp("handle", command) == 0) {
        return 2;
    } else if (strcmp("mask", command) == 0) {
        return 3;
    } else if (strcmp("pending", command) == 0) {
        return 4;
    }
    return 0;
}

