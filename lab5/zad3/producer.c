//
// Created by agwen on 21.04.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// [1] pipe | [2] row | [3] file | [4] N
int main(int argc, char* argv[]){
    if (argc != 5){
        printf("[ERROR] ARGUMENTS BAD\n");
        exit(-1);
    }
    int row = atoi(argv[2]);
    int n = atoi(argv[4]);
    printf("[INFO] Row number: %d\n", row);
    FILE* file = fopen(argv[3], "r");
    if (file == NULL){
        printf("[ERROR] FILE BAD!\n");
        exit(-1);
    }
    FILE* pipe = fopen(argv[1], "w");
    if (pipe == NULL){
        printf("[ERROR] PIPE BAD!\n");
        exit(-1);
    }


    char buffer[n];
    char mess[n + 5];

    while(fread(buffer, sizeof(char), n, file) == n){
        buffer[strlen(buffer) - 1] = '\n';
        snprintf(mess, sizeof(mess), "%d.%s", row, buffer);
        printf("[INFO] PRODUCER WRITE TO PIPE: %s\n", buffer);
        fwrite(mess, sizeof(char), n + 5, pipe);
        sleep(1);
    }
    fclose(file);
    fclose(pipe);
    return 0;
}