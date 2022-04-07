//
// Created by agwen on 05.04.2022.
//

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

void info_handler_1(int sig, siginfo_t *info, void *ucontext);
void handler(int sig);
void fork_exterminate(pid_t child_pid, int sig, int w8);
void test_no_1();
void test_no_2();
void test_no_3();

int main() {
    test_no_1();
    test_no_2();
    test_no_3();
    return 0;
}

void handler(int sig) {
    printf("[INFO] Signal handled, SIG %d, PID %d, PPID %d\n", sig, getpid(), getppid());
}

void info_handler_1(int sig, siginfo_t *info, void *ucontext) {
    printf("[INFO] Signal handled, SIG %d, PID %d, PPID %d\n", sig, getpid(), getppid());
    printf("[INFO]   Signal number:  %d\n"
           "[INFO] Sending process:  %d\n"
           "[INFO]     Error value:  %d\n"
           "[INFO]     Signal code: %d\n"
           "[INFO]      Exit value:  %d\n\n",
           info->si_signo, info->si_pid, info->si_errno, info->si_code, info->si_status);
}

void fork_exterminate(pid_t child_pid, int sig, int w8) {
    child_pid = fork();
    if (child_pid == 0) { // child is doing nothing
        raise(sig);
    } else {
        printf("[INFO] Sending %s to child\n", (sig == 19) ? "SIGSTOP" : "SIGKILL");
        kill(child_pid, sig);
        if (w8) wait(NULL);

    }
}


void test_no_1() {
    printf("+============== TEST CASE 1 ==============+\n");
    printf("+---------------- SA_INFO ----------------+\n");
    struct sigaction action;
    action.sa_sigaction = &info_handler_1;
    action.sa_flags = SA_SIGINFO;

    if (sigaction(SIGUSR1, &action, NULL) == -1) {
        printf("[ERROR] Cannot catch SIGUSR1\n");
        return;
    }
    raise(SIGUSR1);
}

void test_no_2() {
    puts("+============== TEST CASE 2 ==============+");
    puts("+-------------- SA_RESETHAND -------------+");

    struct sigaction action;
    action.sa_handler = &handler;
    sigaction(SIGCHLD, &action, NULL);

    pid_t child_pid = -69; //TODO DO WYJEBANIA
    for (int i = 0; i < 5; i++) {
        fork_exterminate(child_pid, SIGKILL, 1);
    }
    puts("\n[INFO] SA_RESETHAND is ON!");
    action.sa_flags = SA_RESETHAND;
    if (sigaction(SIGCHLD, &action, NULL)  == -1) {
        printf("[ERROR] Problem has occurred\n");
        return;
    }

    for (int i = 0; i < 5; i++) {
        fork_exterminate(child_pid, SIGKILL, 1);
    }


    printf("Handler was used only once, after first iteration default action was restored\n");
}

void test_no_3() {
    puts("+============== TEST CASE 3 ==============+");
    puts("+-------------- SA_NOCLDSTOP -------------+");

    struct sigaction action;
    action.sa_handler = &handler;
    action.sa_flags = 0;
    sigaction(SIGCHLD, &action, NULL);

    pid_t child_pid = -1;
    fork_exterminate(child_pid, SIGSTOP, 0);
    fork_exterminate(child_pid, SIGKILL, 1);

    puts("\n[INFO] SA_NOCLDSTOP is ON!");
    action.sa_flags = SA_NOCLDSTOP;
    sigaction(SIGCHLD, &action, NULL);
    if (sigaction(SIGCHLD, &action, NULL)  == -1) {
        printf("[ERROR] Problem has occurred\n");
        return;
    }

    fork_exterminate(child_pid, SIGSTOP, 0);
    fork_exterminate(child_pid, SIGKILL, 1);

    puts("As expected SIGCHLD from SIGSTOP was ignored after setting SA_NOCLDSTOP\n");

}








