//
// Created by agwen on 23.03.2022.
//

#define _XOPEN_SOURCE 500
#define _DEFAULT_SOURCE
#include <ftw.h>


#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>


#ifdef NFTW
char* get_type(mode_t type);
int walk_func(const char* fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);
#endif

#ifdef DIRE
void print_dir(char* dirname);
void print_file(const char* filename, int type, char* path_name);
char* get_type(int type);
#endif

char* get_time(long int time_sec);


int count[7] = {0, 0, 0, 0, 0, 0, 0};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("[ERROR] invalid number of arguments!\n");
        exit(1);
    }

    printf("| LINKS |  TYPE   |         SIZE         |  LAST ACCESS DATE   |  LAST MODIFICATION  | PATH\n"
           "========================================================================================================================++++\n");
    #ifdef DIRE
    print_dir(argv[1]);
    #endif
    #ifdef NFTW
    nftw(argv[1], walk_func, 200, FTW_PHYS);
    #endif
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

char* get_time(long int time_sec) {
    char* date = calloc(30, sizeof(char));
    strftime(date, 30, "%Y-%m-%d %H:%M:%S", localtime(&time_sec));
    return date;
}

#ifdef NFTW
char* get_type(mode_t type) {
    switch (type & S_IFMT) {
        case S_IFREG:   count[0]++; return "file";
        case S_IFDIR:   count[1]++; return "dir";
        case S_IFCHR:   count[2]++; return "char dev";
        case S_IFBLK:   count[3]++; return "block dev";
        case S_IFIFO:   count[4]++; return "fifo";
        case S_IFLNK:   count[5]++; return "slink";
        case S_IFSOCK:  count[6]++; return "sock";
    }
    return " ";
}

int walk_func(const char* fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    char *buffor = calloc(300, sizeof(char));
    buffor = realpath(fpath, NULL);
    char* last_acc =  get_time(sb->st_atime);
    char* last_mod =  get_time(sb->st_mtime);
    printf("|%6lu | %7s | %20ld | %14s | %14s | %s\n"
           , sb->st_nlink, get_type(sb->st_mode), sb->st_size, last_acc, last_mod, buffor);
    free(buffor);
    free(last_mod);
    free(last_acc);
    return 0;
}
#endif

#ifdef DIRE
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


void print_file(const char* filename, int type, char* path_name) {
    char *buffor = calloc(300, sizeof(char));
    if (strncmp(path_name, "/home", 4) == 0) {
        strcat(buffor, path_name);
//        buffor[strlen(buffor) - 1] = '\0';
    } else {
        getcwd(buffor, 300);
    }
    if (strcmp(filename, ".") != 0 && strcmp(filename, "..") != 0) {
        strcat(buffor, "/");
        strcat(buffor, filename);
    }
//    buffor = realpath(filename, NULL);
    struct stat st;
    lstat(filename, &st);
    char* last_acc =  get_time(st.st_atime);
    char* last_mod =  get_time(st.st_mtime);
    // links, type, size [B], last access, last modification, absolute path
    printf("|%6lu | %7s | %20ld | %14s | %14s | %s", st.st_nlink,get_type(type), st.st_size, last_acc, last_mod, buffor);
    if (type == DT_DIR) {
        printf("/\n");
    } else { printf("\n"); }
    free(last_acc);
    free(last_mod);
    free(buffor);
}


void print_dir(char* dirname) {
    DIR* dir = opendir(dirname);
    if (dir == NULL) {
        printf("[ERROR] could not open directory: %s PP\n", dirname);
        return;
    }

    struct dirent* entity;
    entity = readdir(dir);
    print_file(entity->d_name, entity->d_type, dirname);
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
    free(entity);
    closedir(dir);
}
#endif