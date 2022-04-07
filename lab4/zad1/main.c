//
// Created by agwen on 05.04.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

int get_mode(char* command);
void signal_handler(int signal);
void check_fork(int mode);
void check_parent(int mode);

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("[ERROR] Invalid number of arguments!\n");
        exit(-1);
    }
    printf("+==========================+\n");
    printf("[INFO] Input %s %s\n", argv[1], argv[2]);

    int mode = get_mode(argv[2]);
    check_parent(mode);
    if (strcmp("fork", argv[1]) == 0) {
        if (fork() == 0) {
            check_fork(mode);
        }
    } else if (strcmp("exec", argv[1]) == 0) {
        execl("./t1", "./t1", argv[2], NULL);
    } else {
        printf("[ERROR] Something went wrong! \n");
        exit(-1);
    }

    return 0;
}

void check_parent(int mode) {
    switch (mode) {
        case 1:
            signal(SIGUSR1, SIG_IGN); //ignore sigusr
            raise(SIGUSR1); // send sigusr
            printf("[INFO] Signal ignored (parent)\n");
            break;
        case 2:
            signal(SIGUSR1, signal_handler); //assign handler
            raise(SIGUSR1);
            break;
        case 3: // TODO refactor this, it hurts to look at
        case 4:
            ;sigset_t new_mask; // this semicolon is necessary because...
            sigemptyset(&new_mask);
            sigaddset(&new_mask, SIGUSR1);
            if (sigprocmask(SIG_BLOCK, &new_mask, NULL) < 0) {
                printf("[ERROR] Could not block signal!\n");
            }
            raise(SIGUSR1);
            if ( mode == 4) {
                sigset_t pending_sigs;
                sigpending(&pending_sigs);
                printf("[INFO] Signal pending... (parent)%s\n",
                       sigismember(&pending_sigs, SIGUSR1)?"YAS":"NAY");
            } else {
                printf("[INFO] Signal masked x0\n");
            }
            break;
        default:
            printf("[ERROR] Invalid command!\n");
            exit(-1);
    }
}

void check_fork(int mode) {
    switch (mode) {
        case 1:
            raise(SIGUSR1);
            printf("[INFO] Signal ignored (child)\n");
            break;
        case 2:
            raise(SIGUSR1);
            break;
        case 3:
            raise(SIGUSR1);
            printf("[INFO] Signal masked (child)\n");
            break;
        case 4:
            ;sigset_t pending_sigs;
            sigpending(&pending_sigs);
            printf("[INFO] Signal pending... (child) %s\n",
                   sigismember(&pending_sigs, SIGUSR1)?"YAS":"NAY");
    }
}

// 1 - ignore,
// 2 - handler,
// 3 - mask,
// 4 - pending
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
void signal_handler(int signal) {
    printf("I hereby handle this signal %d, PID %d\n", signal, getpid());
}