//
// Created by agwen on 21.04.2022.
//


#include <unistd.h>
#include <wait.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

#define BUFFER_SIZE 256
void init_file(char* name, int rows);
void many_to_one();
void one_to_many();
void many_to_many();

int main(){

    mkfifo("pipe", 0666);

    printf("=================[ MANY TO ONE ]=================\n");
    many_to_one();

    printf("=================[ ONE TO MANY ]=================\n");
    one_to_many();

    printf("=================[ MANY TO MANY ]=================\n");
    many_to_many();
    printf("==================================================\n");

    return 0;
}

void init_file(char* name, int rows){
    FILE* file = fopen(name, "w");
    if ( file == NULL) {
        printf("[ERROR] EMPTY FILE BAD\n");
        exit(-1);
    }
    char* line = calloc(BUFFER_SIZE, sizeof(char));

    for (int i = 0; i < BUFFER_SIZE - 1; i++){
        line[i] = ' ';
    }
    line[BUFFER_SIZE -1] = '\n';

    for (int i = 0; i < rows - 1; i++){
        fprintf(file, "%s", line);
    }
    line[BUFFER_SIZE - 1] = '\0';
    fprintf(file, "%s", line);

    fclose(file);
    free(line);
}

void many_to_one(){
    char* result = "consumer_files/many_to_one_res.txt";
    char* consumer_N = "10";
    char* producer_N = "5";
    char* input_files[] = {"producer_files/1.txt", "producer_files/2.txt",
                           "producer_files/A.txt", "producer_files/B.txt",
                           "producer_files/C.txt"};
    int producers = 5;

    pid_t pid;
    if (fork() == 0){
        execl("./consumer", "./consumer", "pipe", result, consumer_N, NULL);
        printf("[ERROR] YOU SHOULD NOT BE HERE\n");
    }

    init_file(result, producers);

    for (int i = 0; i < producers; i++){
        pid = fork();
        char row[2];
        sprintf(row,"%d", i);
        if (pid == 0){
            printf("[INFO] %d\n", execl("./producer", "./producer", "pipe", row, input_files[i], producer_N,NULL));
        }
    }

    for (int i = 0; i < producers + 1 ; i++){
        wait(NULL);
    }
}

void one_to_many(){
    char* result = "consumer_files/one_to_many_res.txt";
    char* cons_N = "10";
    char* prod_N = "5";
    char input_file[] = "producer_files/A.txt";
    int consumers = 5;

    for (int i = 0; i < consumers; i++){
        if (fork() == 0){
            execl("./consumer", "./consumer", "pipe", result, cons_N, NULL);
            printf("[ERROR] YOU SHOULD NOT BE HERE\n");
        }
    }
    init_file(result, 1);
    char row[2];
    sprintf(row,"%d", 0);
    if (fork() == 0){
        printf("[INFO] %d\n", execl("./producer", "./producer", "pipe", row, input_file, prod_N,NULL));
    }

    for (int i = 0; i < consumers + 1 ; i++){
        wait(NULL);
    }
}

void many_to_many(){
    char* result = "consumer_files/many_to_many_res.txt";
    char* cons_N = "10";
    char* prod_N = "5";
    char* input_files[] = {"producer_files/1.txt", "producer_files/2.txt",
                           "producer_files/A.txt", "producer_files/B.txt",
                           "producer_files/C.txt"};
    int producers = 5;
    int consumers = 5;
    for (int i = 0; i < consumers; i++){
        if (fork() == 0){
            execl("./consumer", "./consumer", "pipe", result, cons_N, NULL);
            printf("[ERROR] YOU SHOULD NOT BE HERE\n");
        }
    }
    init_file(result, producers);
    for (int i = 0; i < producers; i++){
        char row[2];
        sprintf(row,"%d", i);
        if (fork() == 0){
            printf("[INFO] %d\n", execl("./producer", "./producer", "pipe", row, input_files[i], prod_N,NULL));
        }
    }
    for (int i = 0; i < producers + consumers; i++){
        wait(NULL);
    }
}