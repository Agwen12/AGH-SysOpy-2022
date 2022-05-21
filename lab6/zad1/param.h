//
// Created by agwen on 28.04.22.
//

#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>

#define LIST 103
#define _2ALL 101
#define _2ONE 102
#define STOP 100
#define INIT 104

#define SERVER 1000

#define MAX_CLIENT 10
#define MESSAGE_LENGTH 300

#define TRY(expr, mess) do { \
    if ((expr) == -1) {   \
        printf(mess);     \
    } \
} while(0);

struct contents {
    int sender_id;
    int deceiver_id;
    char text[MESSAGE_LENGTH];
};

struct message {
    long message_type;
    struct contents content;
};



