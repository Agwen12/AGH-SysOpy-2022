//
// Created by agwen on 21.04.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>

#define BUFFER_SIZE 257
void save_to_file(FILE* file, char* message, int row);

// [1] pipe | [2] file | [3] n
int main(int argc, char* argv[]){

    if (argc != 4){
        printf("[INFO] arguments BAD!\n");
        exit(1);
    }
    int n = atoi(argv[3]);

    FILE* pipe = fopen(argv[1], "r");
    if(pipe == NULL) {
        printf("[ERROR] Pipe BAD!\n");
    }

    FILE* file = fopen(argv[2], "r+");
    if(file == NULL) {
        printf("[ERROR] File BAD!\n");
    }

    char buffer[n];

    while(fread(buffer, sizeof(char), n, pipe) == n){
        printf("[INFO] Consumer read: %s", buffer);
        char* arr = strtok(buffer, ".");
        int num = atoi(arr);
        arr = strtok(NULL, "\n");
        char* line;
        sprintf(line, "%s", arr);
        save_to_file(file, line, num);
    }

    fclose(pipe);
    fclose(file);

    return 0;
}
void save_to_file(FILE* file, char* message, int row){
    rewind(file);
    int fd = fileno(file);
    char* buffer = calloc(BUFFER_SIZE, sizeof(char));
    int counter = 0;
    int i;

    flock(fd, LOCK_EX);

    printf("[INFO] Attempt to save the line to the result file row {%d}\n", row);
    while (fgets(buffer,  BUFFER_SIZE, file) != NULL){
        if (counter == row) {
            for (i = 0; i < BUFFER_SIZE - 1; i++) {
                if (buffer[i] == ' ') break;
            }
            fseek(file, row * (BUFFER_SIZE - 1) + i, SEEK_SET);
            printf("[INFO] Writing message: %s\n", message);
            fprintf(file, message, strlen(message) + 1);
            fflush(file);
            break;
        }
        counter++;
    }
    free(buffer);
    flock(fd, LOCK_UN);
}
