//
// Created by agwen on 31.03.2022.
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <string.h>

long double func(long double x);
long double rect_area(long double a, long double b);
void sum_interval(int p_idx, int process_number, long double rect_width);
void write_to_file(int idx, long double res);
long double read_files(int idx);
void start_timer();
void stop_timer();
double get_time(clock_t start, clock_t end);
void print_nice_time();

clock_t start_time, end_time;
struct tms start_time_buffer, end_time_buffer;




int main(int argc, char** argv) {

    if (argc != 3) {
        printf("[ERROR] Invalid number of arguments\n");
    }


    int process_number = atoi(argv[2]);
    long double rect_width = strtof(argv[1], NULL);

    start_timer();
    int pid;
    for (int i = 0; i < process_number; i++) {
        pid = fork();
        if (pid == -1) {
            printf("[ERROR] Something went wrong while creating the process\n");
            exit(1);
        }
        if (pid == 0) { // I'm just a baby
            sum_interval(i, process_number, rect_width);
            break;
        }
    }
    if (pid != 0) { // parent process
        for (int i = 0; i < process_number; ++i) {
            wait(NULL);
        }
        // poczekane
        long double res = 0;
        // tu ma byc zliczanie wyniku
        for (int j = 0; j < process_number; j++) {
            res += read_files(j);
        }
        printf("=======================================\n"
               "PARAMS\nNumber of processes: %d\nRectangle width: %Lf\n",
               process_number, rect_width);
        printf("WYNIK = %Lf\n", res);
        stop_timer();
        print_nice_time();
        printf("=======================================\n\n");

    }
    return 0;
}

long double read_files(int idx) {
    char* buffer = calloc(15, sizeof(char));
    sprintf(buffer, "w%d.txt", idx + 1);
    FILE* f = fopen(buffer, "r+");
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* res_buffer = (char*)malloc(file_size + 1);
    fread(res_buffer, file_size, 1, f);
    long double r;
    r = atof(res_buffer);
    fclose(f);
    free(res_buffer);
    free(buffer);
    return r;
}

long double func(long double x) {
    return 4 / (x*x + 1);
}
long double rect_area(long double a, long double b) {
    return (b - a) * func((b + a) * 0.5);
}
void write_to_file(int idx, long double res){
    char* buffer = calloc(30, sizeof(char));
    sprintf(buffer, "w%d.txt", idx + 1);
    FILE* f = fopen(buffer, "w+");
    fprintf(f, "%Lf", res);
    free(buffer);
    fclose(f);
}

void sum_interval(int p_idx, int process_number, long double rect_width){
    double sum = 0;
    long double start = p_idx / (long double) process_number;
    long double stop = (p_idx + 1) / (long double) process_number;
    while (start + rect_width <= stop) {
        sum += rect_area(start, start + rect_width);
        start += rect_width;
    }
    write_to_file(p_idx, sum);
}

void start_timer() {
    start_time = times(&start_time_buffer);
}
void stop_timer() {
    end_time = times(&end_time_buffer);
}
double get_time(clock_t start, clock_t end) {
    return (double ) (end - start) / (double) sysconf(_SC_CLK_TCK);
}
void print_nice_time(){
    printf("real: %.3fs user: %.3fs sys: %.3fs\n",
           get_time(start_time, end_time),
           get_time(start_time_buffer.tms_cutime, end_time_buffer.tms_cutime),
           get_time(start_time_buffer.tms_cstime, end_time_buffer.tms_cstime));
}
