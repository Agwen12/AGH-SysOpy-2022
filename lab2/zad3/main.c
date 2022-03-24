//
// Created by agwen on 23.03.2022.
//

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

void print_dir(char* dirname);
void print_file(const char* filename, int type, char* path_name);
char* tttime(long int time_sec);
char* get_type(int type);

int count[7] = {0, 0, 0, 0, 0, 0, 0};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("[ERROR] invalid number of arguments!\n");
        exit(1);
    }
    printf("| LINKS |  TYPE   |         SIZE         |  LAST ACCESS DATE   |  LAST MODIFICATION  | PATH\n"
           "========================================================================================================================++++\n");
    print_dir(argv[1]);
    printf("========================================================================================================================++++\n\n");
    printf("+===========================+\n"
           "|#######    STATS    #######|\n"
            "|===========================|\n"
            "|%14d files       |\n"
            "|%14d dirs        |\n"
           "|%14d char devs   |\n"
           "|%14d block devs  |\n"
           "|%14d fifos       |\n"
           "|%14d slinks      |\n"
           "|%14d socks       |\n"
           "+===========================+\n",
           count[0], count[1], count[2], count[3], count[4], count[5], count[6]);
    return 0;
}

char* get_type(int type) {
    switch (type) {
        case DT_REG:    count[0]++; return "file";
        case DT_DIR:    count[1]++; return "dir";
        case DT_CHR:    count[2]++; return "char dev";
        case DT_BLK:    count[3]++; return "block dev";
        case DT_FIFO:   count[4]++; return "fifo";
        case DT_LNK:    count[5]++; return "slink";
        case DT_SOCK:   count[6]++; return "sock";
    }
    return "[NONE]";
}

char* tttime(long int time_sec) {
    char* date = calloc(30, sizeof(char));
    strftime(date, 30, "%Y-%m-%d %H:%M:%S", localtime(&time_sec));
    return date;
}

void print_file(const char* filename, int type, char* path_name) {
    char *buffor = calloc(300, sizeof(char));
    if (strncmp(path_name, "/home", 5) == 0) {
        strcat(buffor, path_name);
        buffor[strlen(buffor) - 2] = '\0';
    } else {
        getcwd(buffor, 300);
    }
    if (strcmp(filename, ".") != 0 && strcmp(filename, "..") != 0) {
        strcat(buffor, "/");
        strcat(buffor, filename);
    }
    struct stat st;
    lstat(filename, &st);
    // links, type, size [B], last access, last modification, absolute path
    printf("|%6lu | %7s | %20ld | %14s | %14s | %s", st.st_nlink,get_type(type), st.st_size, tttime(st.st_atime), tttime(st.st_mtime), buffor);
    if (type == DT_DIR) {
        printf("/\n");
    } else { printf("\n"); }
};


void print_dir(char* dirname) {
    DIR* dir = opendir(dirname);
    if (dir == NULL) {
        printf("[ERROR] could not open directory: %s PP\n", dirname);
        return;
    }

    struct dirent* entity;
    entity = readdir(dir);
//    print_file(entity->d_name, entity->d_type, dirname);
    while (entity != NULL) {
        if (strcmp(entity->d_name, ".") != 0 && strcmp(entity->d_name, "..") != 0) {
            print_file(entity->d_name, entity->d_type, dirname);
        }
        if ((entity->d_type == DT_DIR && strcmp(entity->d_name, ".") != 0 && strcmp(entity->d_name, "..") != 0) && entity->d_type != DT_LNK ) {
            char path[300] = { 0 };
            strcat(path, dirname);
            strcat(path, "/");
            strcat(path, entity->d_name);
            print_dir(path);
        }
        entity = readdir(dir);

    }
    closedir(dir);
}