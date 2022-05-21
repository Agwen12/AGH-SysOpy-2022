//
// Created by agwen on 22.04.2022.
//



#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


void sighandler(int sig, siginfo_t* siginfo, void* context) {
    printf("[INFO] child: %d\n", siginfo->si_value.sival_int);
}

int main(int argc, char* argv[]) {

    if(argc != 3){
        printf("Not a suitable number of program parameters\n");
        return 1;
    }

    struct sigaction action;
    action.sa_sigaction = &sighandler;

    action.sa_flags = SA_SIGINFO;

    sigemptyset(&action.sa_mask);

    sigset_t mask;

    if (sigfillset(&mask) == -1)
        perror("AAAAAAAa\n");

    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGUSR2);

    int child = fork();
    if(child == 0) {
        //zablokuj wszystkie sygnaly za wyjatkiem SIGUSR1 i SIGUSR2
        //zdefiniuj obsluge SIGUSR1 i SIGUSR2 w taki sposob zeby proces potomny wydrukowal
        //na konsole przekazana przez rodzica wraz z sygnalami SIGUSR1 i SIGUSR2 wartosci

        sigprocmask(SIG_BLOCK, &mask, NULL);
        sigaction(SIGUSR1, &action, NULL);
        sigaction(SIGUSR2, &action, NULL);

        pause();
    }
    else {

        sleep(1);

        union sigval sv;
        sv.sival_int = atoi(argv[1]);
        int sig = atoi(argv[2]);
        sigqueue(child, sig, sv);
        //wyslij do procesu potomnego sygnal przekazany jako argv[2]
        //wraz z wartoscia przekazana jako argv[1]
    }

    return 0;
}