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

clock_t start_time;
clock_t stop_time;

struct tms pcu_start;
struct tms pcu_stop;

int is_number(char* arg);
void start_timer();
void stop_timer();

int main(int argc, char** argv) {

    if (argc <= 1) {
        printf("not enough arguments\n");
        exit(-1);
    }

    for (int i = 0; i < argc; ++i) {
//        printf("argc = %d  argv=%s\n", i, argv[i]);
        if(strcmp("create_table", argv[i]) == 0) {
            if (i + 1 >= argc) {
                printf("create_table requires [size] argument\n");
                exit(-1);
            }
            if (is_number(argv[i + 1])) {
                int size = atoi(argv[i + 1]);


//                start_timer();
                create_block_table(size);
//                stop_timer("[OP] CREATE_TABLE\n");
            }
        }
        if (strcmp("wc_command", argv[i]) == 0) {
            if (i + 1 >= argc) {
                printf("wc_command requires at least 2 arguments\n");
                exit(-1);
            }
            int counter = 0;
            int arg_idx = i + 1;
            while (arg_idx < argc && !(
                    strcmp("wc_command", argv[arg_idx]) == 0 ||
                    strcmp("create_table", argv[arg_idx]) == 0 ||
                    strcmp("remove_block", argv[arg_idx]) == 0) ) {

//                printf("a = %d  argv=%s\n", arg_idx, argv[arg_idx]);
                counter++;
                arg_idx++;
            }
            if (counter  < 1) {
                printf("wc_command requires at least 2 arguments\n");
                exit(-1);
            }

//            start_timer();
            wc_command(argv + i + 1, counter);
            int next_block = reserve_block();
            printf("[INFO] block allocated with index: %d\n", next_block);
//            stop_timer("[OP] COUNTING + ALLOCATING\n");
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

//            start_timer();
            remove_block(idx);
//            stop_timer("[OP] REMOVING BLOCK\n");
        }

        if (strcmp("start_timer", argv[i]) == 0) {
            start_timer();
        }

        if (strcmp("stop_timer", argv[i]) == 0) {
            if (i + 1>= argc) {
                printf("[ERROR] stop_timer requires 1 argument\n");
                exit(-1);
            }
            stop_timer(argv[i + 1]);
        }
    }
    free_table();
}

int is_number(char* arg) {
    unsigned long len = strlen(arg);
    for (int i = 0; i < len; ++i) {
        if(!isdigit(arg[i])) {
            printf("Invalid argument, requires int\n");
            return 0;
        }
    }
    return 1;
}

void start_timer() {
    start_time = times(&pcu_start);
}
void stop_timer(char* operation_description) {
    stop_time = times(&pcu_stop);
    int64_t clk_tck = sysconf(_SC_CLK_TCK);

    char* header = "Real Time [s] | System Time [s] | User Time [s] | Description";
    char* times_log = calloc(strlen("Real:\t000.0000s\nUser:\t000.0000s\nSystem:\t000.0000\n") + 1,
                            sizeof(char));
    
    double real_time = (double) (stop_time - start_time) / clk_tck;
    double user_time = (double) (pcu_stop.tms_cutime - pcu_start.tms_cutime);
    double sys_time = (double) (pcu_stop.tms_cstime - pcu_start.tms_cstime);
//    char* times_log = "==========TIMER==========\nReal:\t%.4fs\nUser:\t%.4fs\nSystem:\t%.4fs\n";
    sprintf(times_log, "Real:\t%.4fs\nUser:\t%.4fs\nSystem:\t%.4fs\n",
           real_time, user_time, sys_time);
//    printf("%s %s", operation_description, times_log);
    FILE *fp;
    fp = fopen("raport2.txt", "a");

    if (fp == NULL) {
        system("touch raport2.txt");
        fp = fopen("raport2.txt", "a");
    }
    fputs(operation_description, fp);
    fputs(times_log, fp);
    fclose(fp);
    free(times_log);
}
