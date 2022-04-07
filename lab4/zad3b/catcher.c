//
// Created by agwen on 06.04.2022.
//
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

int S1 = SIGUSR1;
int S2 = SIGUSR2;

pid_t sender_pid = 0;
long messages_received = 0;
long awaited_messages = 0;
int send_back = 0;
int waited_for = 1;

void handler_s1(int sig, siginfo_t *info, void *context);
void handler_s2(int sig, siginfo_t *info, void *context);
void install_handler(void (*f)(int, siginfo_t*, void*), int sig);
void send(char** argv);


int main(int argc, char** argv) {

    if (argc != 2) {
        printf("[ERROR] Invalid number of args\n");
        return -1;
    }
    if (strcmp("sigrt", argv[1]) == 0) {
        S1 = SIGRTMIN+12;
        S2 = SIGRTMIN+24;
    }
    printf("[=============< CATCHER >=============]\n");
    printf("[CATCHER] init...\n");
    printf("[CATCHER] Program has started\n");
    printf("[CATCHER] Program's pid %d\n", getpid());
    install_handler(handler_s1, S1);
    install_handler(handler_s2, S2);
    printf("[CATCHER] waiting for messages...\n\n");


    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, S1);
    sigdelset(&mask, S2);
    while (waited_for) {
        if (waited_for == 0) break;
        sigsuspend(&mask);
    }

    send(argv);

    printf("[=============< STATS >=============]\n");
    printf("[SENDER] %10ld Messages received\n", messages_received);
    printf("[SENDER] %10d Messages send back\n", send_back);

   return 0;
}


void send(char** argv) {
    if (strcmp("sigrt", argv[1]) == 0 || strcmp("kill", argv[1]) == 0) {
        for (int i = 0; i < messages_received; i++) {
            kill(sender_pid, S1);
            send_back++;
        }
        printf("[CATCHER] sending S2 to sender\n");
        kill(sender_pid, S2);
    } else if (strcmp("sigqueue", argv[1]) == 0) {
        union sigval value;
        for (int i = 0; i < messages_received; ++i) {
            value.sival_int = i;
            if (sigqueue(sender_pid, S1, value) < 0) {
                printf("[ERROR] Could not sent signal!\n");
            } else {
                send_back++;
            }
        }
        value.sival_int = send_back;
        if (sigqueue(sender_pid, S2, value) == 0) {
            printf("[CATCHER] S2 send to sender\n");
        } else {
            printf("[ERROR] could not send S2 to sender\n");
        }
    }
}

void install_handler(void f(int, siginfo_t*, void*), int sig) {

    struct sigaction action;
    action.sa_sigaction = f;
    action.sa_flags = SA_SIGINFO;
    sigemptyset(&action.sa_mask);
    if (sigaction(sig, &action, NULL) == -1) {
        printf("[ERROR] something went wrong while setting handler for %s!\n", (sig == SIGUSR1)?"SIGUSR1":"SIGUSR2");
        exit(-1);
    }
    printf("[CATCHER] handler for %s installed successfully\n", (sig == SIGUSR1)?"SIGUSR1":"SIGUSR2");
}


void handler_s1(int sig, siginfo_t *info, void *context) {
    if(info->si_code == SI_USER || info->si_code == SI_QUEUE) {
        messages_received++;
    }
}

void handler_s2(int sig, siginfo_t *info, void *context) {

    if (info->si_code == SI_USER || info->si_code == SI_QUEUE) {
        printf("[CATCHER] received SIGUSER2\n");
        sender_pid = info->si_pid;
        waited_for = 0;
        raise(S2);
    }
}
