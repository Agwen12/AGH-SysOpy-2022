
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#include "param.h"

void send_stop();
void terminate_server();
void handler(int signum);
void log_to_file(int type, int id);
void INITp(struct message msg);
void LISTp(struct message msg);
void STOPp(struct message msg);
void _2ONEp(struct message msg);
void _2ALLp(struct message msg);
void ev_message(struct message msg);

FILE* log_file;
int is_client_free[MAX_CLIENT];
int client_queueueueu[MAX_CLIENT];
int q_id;
int stat_srv = 1;

int main() {
    char* homedir = getenv("HOME");
    key_t queue_key;
    TRY(queue_key = ftok(homedir, 1), "[ERROR] cannnot main KEY\n");
    TRY(q_id = msgget(queue_key, IPC_CREAT | 0600), "[ERROR] cannot main queueu\n");
    for (int i = 0; i < MAX_CLIENT; i++) {
        is_client_free[i] = 1;
    }
    log_file = fopen("./report.txt", "w");
    if (log_file == NULL) {
        printf("[ERROR] cannot opern file\n");
        exit(-1);
    }
    printf("[INFO] YEAh we did it server STARTED!!\n");
    atexit(terminate_server);
    struct sigaction action;
    action.sa_handler = handler;
    sigaction(SIGINT, &action, NULL);
    struct message msg;
    while(stat_srv) {
        if(msgrcv(q_id, &msg, sizeof msg.content, -200, 0) == -1) {
            printf("[ERROR] cannot receive message\n");
            exit(-1);
        }
        ev_message(msg);
    }
}

void handler(int signum) {
    printf("AAAA %d\n", signum);
    exit(1);
}

void ev_message(struct message msg) {
    switch (msg.message_type) {
        case LIST:
            LISTp(msg);
            break;
        case INIT:
            INITp(msg);
            break;
        case _2ONE:
            _2ONEp(msg);
            break;
        case STOP:
            STOPp(msg);
            break;
        case _2ALL:
            _2ALLp(msg);
            break;
        default:
            printf("[ERROR] dunno this type %lu.\n", msg.message_type);
            exit(-1);
    }
    log_to_file(msg.message_type, msg.content.sender_id);
}

void INITp(struct message msg) {
    int c_idx;
    for (int i = 0; i < 5; i++) {
        if(is_client_free[i]) {
            is_client_free[i] = 0;
            c_idx = i;
            break;
        }
    }
    client_queueueueu[c_idx] = atoi(msg.content.text);
    printf("[INFO] New client: %d.\n", c_idx);
    struct message c_msg;
    c_msg.message_type = INIT;
    sprintf(c_msg.content.text, "%d", c_idx);
    TRY(msgsnd(client_queueueueu[c_idx], &c_msg, sizeof c_msg.content, 0),
        "[ERROR] cannot send message.\n");
}
void LISTp(struct message msg) {
    struct message c_msg;
    c_msg.message_type = LIST;
    char buffer[250] = "[INFO] Online users: ";
    char client[4];
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (!is_client_free[i]) {
            sprintf(client, "%d ", i);
            strcat(buffer, client);
        }
    }
    strcpy(c_msg.content.text, buffer);
    c_msg.content.sender_id = SERVER;
    TRY(msgsnd(client_queueueueu[msg.content.sender_id], &c_msg, sizeof msg.content, 0),
        "[ERROR] cannot send message.\n");
}

void STOPp(struct message msg) {
    is_client_free[msg.content.sender_id] = 1;
    client_queueueueu[msg.content.sender_id] = -1;
    printf("[INFO] Client %d is gone\n", msg.content.sender_id);
}

void _2ONEp(struct message msg ) {
    if (!is_client_free[msg.content.deceiver_id]) {
        TRY(msgsnd(client_queueueueu[msg.content.deceiver_id], &msg, sizeof msg.content, 0),
            "[ERROR] cannot send message.\n");
    }
}

void _2ALLp(struct message msg) {
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (i != msg.content.sender_id && !is_client_free[i]) {
            TRY(msgsnd(client_queueueueu[i], &msg, sizeof msg.content, 0),
                "[ERROR] cannot send message.\n");
        }
    }
}

void terminate_server() {
    send_stop();
    TRY(msgctl(q_id, IPC_RMID, NULL), "[ERROR] cannot remove main queueue.\n");
    fclose(log_file);
}
void send_stop() {
    struct message msg;
    msg.message_type = STOP;
    msg.content.sender_id = SERVER;
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (!is_client_free[i]) {
            TRY(msgsnd(client_queueueueu[i], &msg, sizeof msg.content, 0), "[ERROR] cannot send messgae.\n");
        }
    }
}
void log_to_file(int type, int id) {
    struct msqid_ds info;
    msgctl(q_id, IPC_STAT, &info);
    struct tm tm = *localtime(&info.msg_rtime);
    char buffer[250];
    sprintf(buffer, "time: %d-%02d-%02d %02d:%02d:%02d, id: %d, type: %d\n",
            tm.tm_year + 1900,
            tm.tm_mon + 1,
            tm.tm_mday,
            tm.tm_hour,
            tm.tm_min,
            tm.tm_sec,
            id,
            type
            );
    int end;
    for (int i = 0; i < MESSAGE_LENGTH; i++) {
        if(buffer[i] == '\n') {
            end = i + 1;
            break;
        }
    }
    fwrite(buffer, end * sizeof(char), 1, log_file);
}

