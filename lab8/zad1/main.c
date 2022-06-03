//
// Created by agwen on 13.05.22.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>
#include "misc.h"

int** image;
int** negative;
int width, height, thread_num;

void load_image(char* filename);
void* variant_1(void* arg);
void* variant_2(void* arg);
void write_negative(char* filename);
void create_threads(pthread_t* threads, int* threads_idxs, char* method);
long unsigned int get_time(struct timeval end, struct timeval start);
int var_1_to(int idx);
int var_2_to(int idx);

int main(int argc, char** argv) {
//    pthread_t hello_world_thread;
    if (argc != 5) {
        printf("[ERROR] Invalid number of arguments\n");
        exit(-1);
    }
    thread_num = atoi(argv[1]);
    char* method = argv[2];
    char* input = argv[3];
    char* output = argv[4];
    printf("[INFO] thread_num: {%d}  method: {%s}\n", thread_num, method);
    printf("[INFO] input: {%s}  output: {%s}\n", input, output);

    load_image(input);
    printf("[INFO] Creating negative\n");
    negative = calloc(height, sizeof(int*));
    for (int i = 0; i < height; ++i) {
//        printf("[INFO] Allocating row: %d\n", i);
        negative[i] = calloc(width, sizeof(int));
    }
    printf("[INFO] Negative allocated successfully\n");
    pthread_t* threads = calloc(thread_num, sizeof(pthread_t));
    printf("[INFO] threads allocated\n");
    int* threads_idxs = calloc(thread_num, sizeof(int));
    printf("[INFO] threads idxs allocated\n");

    struct timeval end, start;
    gettimeofday(&start, NULL);
    printf("[INFO] Creating threads\n");
    create_threads(threads, threads_idxs, method);

    FILE* times_file = fopen("Times.txt", "a");

    printf("#====================[ INFO ]====================#\n");
    printf("[INFO] THREADS:\t%d\n", thread_num);
    printf("[INFO] VARIANT:\t%s\n", method);
    printf("[INFO] IMAGE:\t%s\n", input);

    fprintf(times_file, "#====================[ INFO ]====================#\n");
    fprintf(times_file, "[INFO] THREADS:\t%d\n", thread_num);
    fprintf(times_file, "[INFO] VARIANT:\t%s\n", method);
    fprintf(times_file, "[INFO] IMAGE:\t%s\n", input);

    for (int i = 0; i < thread_num; ++i) {
        long unsigned int* t;
        pthread_join(threads[i], (void **) &t);
        printf("[INFO] THREAD: %4d time: %5lu [μs]\n", i, *t);
        fprintf(times_file, "[INFO] THREAD: %4d time: %5lu [μs]\n", i, *t);
    }

    gettimeofday(&end, NULL);
    long unsigned int* t = malloc(sizeof(long unsigned int));
    *t = get_time(end, start);
    printf("[INFO] TOTAL TIME: %lu [μs]\n", *t);
    fprintf(times_file, "[INFO] TOTAL TIME: %lu [μs]\n", *t);
    fprintf(times_file, "#================================================#\n\n");


    write_negative(output);
    fclose(times_file);
    for (int i = 0; i < height; ++i) {
        free(image[i]);
        free(negative[i]);
    }

    free(image);
    free(negative);

    return 0;
}

void load_image(char* filename) {
    FILE* f = fopen(filename, "r+");
    TRY_VAL(f, NULL, "Cannot open file");

    /*
     *  "magic number"
     *  {width} {height}
     *  {max_val}
     *  int[width * height]
     */

    char* buffer = calloc(ROW_LENGTH, sizeof(char));

    // skip
    fgets(buffer, ROW_LENGTH, f);
    int got_m = 0, read_img = 0;

    while ((1 - read_img) && fgets(buffer, ROW_LENGTH, f)) {
        if(buffer[0] == '#') continue;
        else if (1 - got_m) {
            sscanf(buffer, "%d %d\n", &width, &height);
            printf("[INFO] width: %d, height: %d\n", width, height);
            got_m = 1;
        } else {
            int max_val;
            sscanf(buffer, "%d \n", &max_val);
            if (max_val != 255) {
                printf("[ERROR] max grey value has to be 255\n");
                exit(-1);
            }
            read_img = 1;
        }
    }

    // reading grey value
    image = calloc(height, sizeof(int*));
    for (int i = 0; i < height; ++i) {
        image[i] = calloc(width, sizeof(int));
    }

    int grey_val;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            fscanf(f, "%d", &grey_val);
            image[i][j] = grey_val;
        }
    }

    fclose(f);
    printf("[INFO] file read successfully\n");
}


long unsigned int get_time(struct timeval end, struct timeval start) {
    return (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
}

int var_1_to(int idx) {
    return (idx != thread_num - 1) ? ((ROW_LENGTH / thread_num) * (idx + 1)) : ROW_LENGTH;
}

void* variant_1(void* arg) {
    struct timeval end, start;
    gettimeofday(&start, NULL);
    int idx = *((int *) arg);

    int from, to;
    from = (ROW_LENGTH / thread_num) * idx;
    to = var_1_to(idx);
    int grey_val;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            grey_val = image[i][j];
            if(grey_val >= from && grey_val < to) {
                negative[i][j] = 255 - grey_val;
            }
        }
    }
    gettimeofday(&end, NULL);
    long unsigned int* t = malloc(sizeof(long unsigned int));
    *t = get_time(end, start);
    pthread_exit(t);
}


int var_2_to(int idx) {
    return (idx != thread_num - 1) ? ((idx + 1) * ceil(width / thread_num) - 1) : width - 1;
}

void* variant_2(void* arg) {
    struct timeval end, start;
    gettimeofday(&start, NULL);
    int idx = *((int*) arg);

    int x_from, x_to;
    x_from = idx * ceil(width / thread_num);
    x_to = var_2_to(idx);

    int grey_val;
    for (int i = 0; i < height; ++i) {
        for (int j = x_from; j <= x_to; ++j) {
            grey_val = image[i][j];
            negative[i][j] = 255 - grey_val;
        }
    }


    gettimeofday(&end, NULL);
    long unsigned int *t = malloc(sizeof(long unsigned int));
    *t = get_time(end, start);
    pthread_exit(t);
}

void write_negative(char* filename) {
    FILE *f = fopen(filename, "w+");
    TRY_VAL(f, NULL, "Cannot open file");

    fprintf(f, "P2\n%d %d\n255\n", width, height);

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            fprintf(f, "%d ", negative[i][j]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

void create_threads(pthread_t* threads, int* threads_idxs, char* method) {
    for (int i = 0; i < thread_num; ++i) {
        threads_idxs[i] = i;
        if(!strcmp("variant_1", method)) {
            pthread_create(&threads[i], NULL, &variant_1, &threads_idxs[i]);
        } else if (!strcmp("variant_2", method)) {
            pthread_create(&threads[i], NULL, &variant_2, &threads_idxs[i]);
        }
        printf("[INFO] Created thread with id: %d\n", i);
    }
}
