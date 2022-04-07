//
// Created by agwen on 06.04.2022.
//
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

int S1 = SIGUSR1;
int S2 = SIGUSR2;

pid_t catcher_pid = 0;
int messages_received = 0;
int waited_for = 1;
int catcher_send = 0;

void handler_s1(int sig, siginfo_t *info, void *context);
void handler_s2(int sig, siginfo_t *info, void *context);
void install_handler(void (*f)(int, siginfo_t*, void*), int sig);
void send(char** argv, int spam);


// params catcher-PID, num_of_messages, mode
int main(int argc, char** argv) {
    if (argc != 4) {
        printf("[ERROR] Invalid number of arguments!\n");
        return -1;
    }
    if (strcmp("sigrt", argv[3]) == 0) {
        S1 = SIGRTMIN+12;
        S2 = SIGRTMIN+24;
    }
    catcher_pid = atoi(argv[1]);
    int spam = atoi(argv[2]);

    printf("[=============< SENDER >=============]\n"
           "[SENDER] init...\n"
           "[SENDER] Program has started\n"
           "[SENDER] Sender pid %d\n", getpid());
    install_handler(handler_s1, S1);
    install_handler(handler_s2, S2);

    send(argv, spam);

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, S1);
    sigdelset(&mask, S2);

    while (waited_for) {
        if (waited_for == 0) break;
        sigsuspend(&mask);
    }

    printf("[=============< STATS >=============]\n"
    "[SENDER] %10d Messages send\n"
    "[SENDER] %10d Messages received\n",
     spam, messages_received);

}

void send(char** argv, int spam) {
    if (strcmp("sigrt", argv[3]) == 0 || strcmp("kill", argv[3]) == 0) {
        for (int i = 0; i < spam; ++i) {
            kill(catcher_pid, S1);
        }
        printf("[SENDER] sending S2 to the catcher\n");
        kill(catcher_pid, S2);
    } else if (strcmp("sigqueue", argv[3]) == 0) {
        union sigval value;
        for (int i = 0; i < spam; ++i) {
            value.sival_int = i;
            value.sival_ptr = "This is a sender message!";
            if (sigqueue(catcher_pid, S1, value) != 0) {
                printf("[SENDER] Could not sent message!\n");
            }
        }
        value.sival_int = messages_received;
        value.sival_ptr = "This is my FINAL message!";
        printf("[SENDER] sending S2 to catcher\n");
        sigqueue(catcher_pid, S2, value);
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
    printf("[SENDER] handler for %s installed successfully\n", (sig == SIGUSR1)?"SIGUSR1":"SIGUSR2");
}


void handler_s1(int sig, siginfo_t *info, void *context) {
    if(info->si_code == SI_USER || info->si_code == SI_QUEUE) {
        messages_received++;
    }
}

void handler_s2(int sig, siginfo_t *info, void *context) {
    if (info->si_code == SI_USER || info->si_code == SI_QUEUE) {
        printf("[SENDER] received S2\n");
        if (info->si_code ==  SI_QUEUE) {
            printf("[SENDER] inf form info: %d\n", info->si_value.sival_int);
        }
        waited_for = 0;
    }
}
