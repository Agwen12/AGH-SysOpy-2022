
#include <fcntl.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "param.h"

int q_id;
int c_id;
int usr_id;
struct message snd_msg;
struct message rcv_msg;
int stat_srv = 1;
pid_t ctc_pid = 0;

void msg_send() {
    TRY(msgsnd(q_id, &snd_msg, sizeof snd_msg.content, 0),
        "[ERROR] cannot send message\n");
}
void msg_receive() {
    TRY(msgrcv(c_id, &rcv_msg, sizeof rcv_msg.content, -3000, 0),
        "[ERROR] Cannot receive msg\n");
}

void handler(int signum) {
    printf("AAA %d", signum);
    exit(1);
}

void eval_comm(char buffer[250], char* comm[3]) {
    int beg = 0, idx = 0;
    for (int i = 0; i < 250; i++) {
        if (idx == 3) {
            break;
        }
        if (buffer[i] == ' ' || buffer[i] == '\n') {
            comm[idx] = calloc(i - beg + 1, sizeof (char));
            memcpy(comm[idx], buffer + beg, (i - beg) * sizeof (char));
            beg = i + 1;
            idx += 1;
            if (buffer[i] == '\n') break;
        }
    }
}

//done
void ev_STOP() {
    kill(getppid(), SIGTERM);
    exit(1);
}

//done
void stop_catching() {
    TRY(msgctl(c_id, IPC_RMID, NULL), "[ERROR] cannont remove queueueu.\n");
}


//done
void LIST_send() {
    snd_msg.message_type = LIST;
    msg_send();
}
//done
void _2ALL_send(char* command[3]) {
    if (!command[1]) {
        printf("[ERROR] Invalid number of arguments.\n");
        return;
    }
    snd_msg.message_type= _2ALL;
    snd_msg.content.text[0] = '\0';
    strcpy(snd_msg.content.text, command[1]);
    msg_send();
}
//done
void _2ONE_send(char* command[3]) {
    if (!command[1] || !command[2]) {
        printf("[ERROR] Invaid number of arguments.\n");
        return;
    }
    snd_msg.message_type = _2ONE;
    snd_msg.content.text[0] = '\0';
    strcpy(snd_msg.content.text, command[2]);
    snd_msg.content.deceiver_id = atoi(command[1]);
    msg_send();
}

//done
void STOP_send() {
    kill(ctc_pid, SIGINT);
    raise(SIGINT);
}

//done
void stop_sending() {
    snd_msg.message_type = STOP;
    msg_send();
}

void send_parse(char* command[3]) {
    if (strcmp(command[0], "LIST") == 0) LIST_send();
    else if (strcmp(command[0], "2ALL") == 0) _2ALL_send(command);
    else if (strcmp(command[0], "2ONE") == 0) _2ONE_send(command);
    else if (strcmp(command[0], "STOP") == 0) STOP_send();
    else printf("[INFO] Dunno this message - %s.\n", command[0]);
}

void catch() {
    atexit(stop_catching);
    while(stat_srv) {
        TRY(msgrcv(c_id, &rcv_msg, sizeof rcv_msg.content, -500, 0), "[ERROR] Cannot receive message\n");
        if (rcv_msg.content.sender_id == SERVER) {
            if (rcv_msg.message_type != STOP) {
                printf("%s\n", rcv_msg.content.text);
            }
            else {
                ev_STOP();
            }
        }
        else {
            struct msqid_ds info;
            msgctl(c_id, IPC_STAT, &info);
            struct tm tm = *localtime(&info.msg_rtime);
            printf("[INFO] %d-%02d-%02d %02d:%02d:%02d - GOT message from %d: %s\n",
                   tm.tm_year + 1900,
                   tm.tm_mon + 1,
                   tm.tm_mday,
                   tm.tm_hour,
                   tm.tm_min,
                   tm.tm_sec,
                   rcv_msg.content.sender_id,
                   rcv_msg.content.text);
        }
    }
}

void sending() {
    atexit(stop_sending);
    char buffer[MESSAGE_LENGTH];
    int idx = 0;
    while(stat_srv) {
        while ((buffer[idx] = fgetc(stdin)) != '\n') {
            idx += 1;
        }
        char* comm[3];
        for (int i = 0; i < 3; i++) comm[i] = NULL;

        eval_comm(buffer, comm);
        send_parse(comm);

        for (int i = 0; i < 3; i++) {
            if(comm[i]) free(comm[i]);
        }
        idx = 0;
        sleep(1);
    }
}

void connect() {
    char *homedir = getenv("HOME");
    key_t queue_key;
    TRY(c_id = msgget(IPC_PRIVATE, 0600), "[ERROR] cannot client queueue\n");
    TRY(queue_key = ftok(homedir, 1), "[ERROR] Cannot main queueu KEY\n");
    TRY(q_id = msgget(queue_key, 0), "[ERROR] cannot main queueue\n");
    snd_msg.message_type = INIT;
    sprintf(snd_msg.content.text, "%d", c_id);
    msg_send();
    msg_receive();
    usr_id = atoi(rcv_msg.content.text);
    snd_msg.content.sender_id = usr_id;
    printf("[INFO] I was welcome and granted session with id: %d\n", usr_id);
}

int main() {
    struct sigaction sa;
    connect();
    sa.sa_handler = handler;
    sigaction(SIGINT, &sa, NULL);
    ctc_pid = fork();
    if (!ctc_pid) {
        catch();
    }
    else {
        sending();
    }
}