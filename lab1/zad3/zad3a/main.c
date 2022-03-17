//
// Created by agwen on 15.03.2022.
//
#include "wclibrary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/times.h>

#ifdef DYNAMIC
    #include "wclibrary_dynamic.h"
#endif

clock_t start_time;
clock_t stop_time;

struct tms pcu_start;
struct tms pcu_stop;

int is_number(char* arg);
void start_timer();
void stop_timer(char* operation_description);
void init_report();
void print_report();

int main(int argc, char** argv) {
    if (argc <= 1) {
        printf("[ERROR] Not enough arguments\n");
        exit(-1);
    }
#ifdef DYNAMIC
    init_handle();
#endif

    init_report();
    for (int i = 0; i < argc; ++i) {
        if(strcmp("create_table", argv[i]) == 0) {
            if (i + 1 >= argc) {
                printf("[ERROR] create_table requires 1 argument\n");
                exit(-1);
            }
            if (is_number(argv[i + 1])) {
                int size = atoi(argv[i + 1]);

                create_block_table(size);
            }
        }
        if (strcmp("wc_command", argv[i]) == 0) {
            if (i + 1 >= argc) {
                printf("[ERROR] wc_command requires at least 2 arguments\n");
                exit(-1);
            }
            int counter = 0;
            int arg_idx = i + 1;
            while (arg_idx < argc && !(
                    strcmp("wc_command", argv[arg_idx]) == 0 ||
                    strcmp("create_table", argv[arg_idx]) == 0 ||
                    strcmp("remove_block", argv[arg_idx]) == 0 ||
                    strcmp("start_timer", argv[arg_idx]) == 0 ||
                    strcmp("stop_timer", argv[arg_idx]) == 0)) {

                counter++;
                arg_idx++;
            }
            if (counter  < 1) {
                printf("[ERROR] wc_command requires at least 2 arguments\n");
                exit(-1);
            }

            wc_command(argv + i + 1, counter);
        }

        if (strcmp("remove_block", argv[i]) == 0) {
            if (i + 1 >= argc) {
                printf("remove_block requires 1 argument\n");
                exit(-1);
            }
            if (!is_number(argv[i + 1])) {
                printf("remove_block requires integer as an argument\n");
                exit(-1);
            }
            int idx = atoi(argv[i + 1]);

            remove_block(idx);
        }

        if (strcmp("start_timer", argv[i]) == 0) {
            start_timer();
        }

        if (strcmp("stop_timer", argv[i]) == 0) {
            if (i + 1 >= argc) {
                printf("[ERROR] stop_timer requires 1 argument\n");
                exit(-1);
            }
            stop_timer(argv[i + 1]);
        }

        if (strcmp("allocate", argv[i]) == 0) {
            int next_block = reserve_block();
            printf("[INFO] Block allocated with index: %d\n", next_block);
        }
    }
    free_table();

#ifdef DYNAMIC
    free_handle();
#endif
    print_report();
}

int is_number(char* arg) {
    unsigned long len = strlen(arg);
    for (int i = 0; i < len; ++i) {
        if(!isdigit(arg[i])) {
            printf("[ERROR] Invalid argument, requires int\n");
            return 0;
        }
    }
    return 1;
}

void start_timer() {
    start_time = times(&pcu_start);
}

void init_report() {
    FILE *fp;

    fp = fopen("result3a.txt", "a");
    if (fp == NULL) {
        system("touch result3a.txt");
        fp = fopen("result3a.txt", "a");
    }
    char* header = "Real Time [s] | System Time [s] | User Time [s] | Description\n"
                   "==============|=================|===============|============\n";
    fputs( header, fp);
    fclose(fp);
}
void stop_timer(char* operation_description) {
    stop_time = times(&pcu_stop);
    int64_t clk_tck = sysconf(_SC_CLK_TCK);
    double real_time = (double) (stop_time - start_time) / clk_tck;
    double user_time = (double) (pcu_stop.tms_cutime - pcu_start.tms_cutime);
    double sys_time = (double) (pcu_stop.tms_cstime - pcu_start.tms_cstime);
    FILE *fp;
    fp = fopen("result3a.txt", "a");
    fprintf(fp, " %12f | %15f | %13f | %s\n",
            real_time, user_time, sys_time, operation_description);
    fclose(fp);
}
void print_report() {
    system("cat result3a.txt");
}
