//
// Created by agwen on 31.03.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/wait.h>

int is_txt(char* path);
int contains_string(char* file_name, char* string);

void recursive_search(char* path_dir, char* string, int max_rec, int curr_rec, char* path_relative);

int main(int argc, char** argv) {
    if (argc != 4) {
        printf("[ERROR] Invalid number of arguments!\n");
        return 0;
    }
    printf("\n");
    recursive_search(argv[1], argv[2], atoi(argv[3]), 0, "");
    return 0;
}

void recursive_search(char* path_dir, char* string, int max_rec, int curr_rec, char* path_relative) {

    DIR* dir = opendir(path_dir);
    if (dir == NULL) {
        printf("[ERROR] could not open directory: %s PP\n", path_dir);
        exit(-1);
    }

    struct dirent* entity;
    entity = readdir(dir);
    char* path_file = calloc(1000, sizeof(char));
    char* relative_path_file = calloc(1000, sizeof(char));
    int counter = 0;
    pid_t pid;
    while (entity != NULL) {
        if (strcmp(entity->d_name, ".") != 0 &&
            strcmp(entity->d_name, "..") != 0) {
            snprintf(relative_path_file, 1000, "%s/%s", path_relative, entity->d_name);
            snprintf(path_file, 1000, "%s/%s", path_dir, entity->d_name);
            if ((entity->d_type == DT_REG) && is_txt(path_file)) { // I'm a text file
                if (contains_string(path_file, string) == 1) {
                    printf("PID: %d\tpath: %s\n", getpid(), relative_path_file);
                }
            }

            if (entity->d_type == DT_DIR && (curr_rec < max_rec)) {
                counter++;
                pid = fork();
                if (pid == 0) {
                    recursive_search(path_file, string, max_rec, curr_rec + 1, relative_path_file);
                    return;
                }

            }
        }
        entity = readdir(dir);
    }
    if(pid != 0) { // parent
        free(relative_path_file);
        free(path_file);
        for (int i = 0; i < counter; ++i) {
            wait(NULL);
        }
    }
    free(entity);
    closedir(dir);

}
int is_txt(char* path) {
    char* extension = strrchr(path, '.');
    if (extension != NULL && strcmp(extension, ".txt") == 0) {
        return 1;
    }
    return 0;
}
int contains_string(char* file_name, char* string) {
    FILE* fp = fopen(file_name, "r+");
    if (fp == NULL) {
        printf("[ERROR] could not open file: %s\n", file_name);
        return 1;
    }
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* buffer = calloc(size, sizeof(char));
    fread(buffer, sizeof(char ), size, fp);
    char* is_there = strstr(buffer, string);
    free(buffer);
    fclose(fp);
    if (is_there != NULL) {
        return 1;
    }
    return 0;
}