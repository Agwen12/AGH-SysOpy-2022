//
// Created by agwen on 20.04.2022.
//
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "unistd.h"

#define BUFFER_SIZE 2048
#define MAX_NAME_SIZE 100
#define MAX_COMP 10 // max number of components
#define MAX_COMMAND 10 // commands in component
#define MAX_ARG_SIZE 40 // max char of each arg

typedef struct component_t {
    pid_t pid;
    int arg_count;
    char*** args;
    char* name;
} component_t;


component_t** parse_input(FILE* f);
void print_components(component_t** components);

void run_commands(component_t** components, int len, int com_counter) {
    int pipes[com_counter + len][2];
    for(int i = 0; i < com_counter + len; i++) {
        if (pipe(pipes[i]) < 0) {
            printf("[ERROR] PIPE ERROR!\n");
            exit(-1);
        }
    }
    printf("================{ COMMANDS }================\n");

    printf("com_counter = %d\n", com_counter);
    printf("len = %d\n", len + com_counter);
    for (int i = 0; i < len; i++) {
        printf("[INFO] comp: %d\n", i);
        for (int j = 0; j <= components[i]->arg_count; j++){
            printf("[INFO]      part %d\n", j);
            int k = 0;
            char* s = "1";
            while (strlen(s) != 0) {
                s = components[i]->args[j][k];
                printf("[INFO]             {%s}\n", s);
                k++;
                s = components[i]->args[j][k];
            }
        }
    }

    for(int comp = 0; comp < len; comp++) {

        for (int part = 0; part <= components[comp]->arg_count; part++) {

            int i = comp + part;
            pid_t pid = fork();
            printf("[%d] comp=%d\tpart=%d\n", getpid(), comp, part);
            if (pid < 0) {
                printf("[ERROR] Cannot fork because...\n");
                exit(-12);
            }
            if (pid == 0) {
                if (i != 0) {
                    dup2(pipes[i - 1][0], STDIN_FILENO);
                }

                if (i != len + com_counter - 1) {
                    dup2(pipes[i][1], STDOUT_FILENO);
                }

                for ( int j = 0; j < len + com_counter - 1; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }
                char** c = components[comp]->args[part];
                int o = 0;
                char* arg[MAX_ARG_SIZE];
                char* s = "1";
                while (strlen(s) != 0)  {
                    arg[o] = c[o];
                    printf("%s\n", c[o]);
                    o++;
                    s = c[o];

                }
                arg[o + 1] = NULL;
                for(int l = 0; l < o + 1; l++)
                    printf("{%s}\n", arg[l]);
                execvp(arg[0], arg);
                exit(0);
            }
        }
    }
}


int main(int argc, char** argv) {

    FILE* f = fopen(argv[1], "r+");
    component_t** components = parse_input(f);


    char* buffer = calloc(1, BUFFER_SIZE);
    component_t** to_run = calloc(sizeof(component_t*), MAX_COMP);
    int comp_counter = 0;
    int command_counter = 0;
    while (fgets(buffer, BUFFER_SIZE, f) != NULL) {
        char* tok;
        tok = strtok(buffer, " \n");
        while (tok != NULL) {
            if (strcmp("|", tok) != 0) {
                for ( int i = 0; i < MAX_COMP; i++) {
//                    printf("%s   %s  %d\n", tok, components[i]->name, strcmp(tok, components[i]->name));
                    if (strcmp(tok, components[i]->name) == 0) {
                        to_run[comp_counter] = components[i];
                        comp_counter++;
                        printf("part_counter = %d\n", components[i]->arg_count);
                        command_counter += components[i]->arg_count;
                        break;
                    }
                }
            }
            tok = strtok(NULL, " \n");
        }
        run_commands(to_run, comp_counter, command_counter);
        command_counter = 0;
        comp_counter = 0;
    }
//    print_components(components);
}
void print_components(component_t** components) {
    for ( int i = 0; i < MAX_COMP; i++) {
        if (strlen(components[i]->name) == 0) break;
        printf("Name: %s \n", components[i]->name);
        printf("arg count: %d\n", components[i]->arg_count);
        for ( int j = 0; j < MAX_COMMAND; j++){
            if (strlen(components[i]->args[j][0]) == 0) break;
            printf("%d ", j);
            for (int k = 0; k < MAX_COMMAND; k++) {
                if (strlen(components[i]->args[j][k]) == 0) break;
                printf("%s ", components[i]->args[j][k]);
            }
            printf("\n");
        }
    }
}

component_t** parse_input(FILE* f) {
    component_t** components = malloc(sizeof(component_t*) * MAX_COMP);

    for (int i = 0; i < MAX_COMP; i++) {
        components[i] = malloc(sizeof(component_t));
        components[i]->name = calloc(sizeof(char), MAX_NAME_SIZE);
        components[i]->args = malloc(sizeof(char**) * MAX_COMMAND);
        for (int j = 0; j < MAX_COMMAND; j++) {
            components[i]->args[j] = malloc(sizeof(char*) * MAX_COMMAND);
            for (int k = 0; k < MAX_COMMAND; k++) {
                components[i]->args[j][k] = calloc(1, MAX_ARG_SIZE);
            }
        }
    }

    char* buffer = calloc(1, BUFFER_SIZE);
    int l = 0;
    while (fgets(buffer, BUFFER_SIZE, f) != NULL && strcmp(buffer, "\n") != 0) {
        char* tok;
        int command_count = 0;
        int arg_count= 0;
        int first = 1;
        tok = strtok(buffer, " \n");
        while (tok != NULL) {
            if (first) {
                strcat(components[l]->name, tok);
                first = 0;
            } else if (strcmp(tok, "|") == 0) {
                command_count++;
                arg_count=0;
            } else if (strcmp(tok, "=") != 0) {
                components[l]->args[command_count][arg_count][0] = '\0';
                strcpy(components[l]->args[command_count][arg_count], tok);
                arg_count++;
            }
            tok = strtok(NULL, " \n");
        }
        components[l]->arg_count= command_count;
        l++;
    }
    return components;
}