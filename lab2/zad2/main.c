
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>


#ifdef LIB
int is_line_empty(FILE* scr, char *c);
void del_n_copy(char *c, char* file);
#endif

#ifdef SYS
#include <fcntl.h>
#include <unistd.h>
int is_line_empty(int scr, char* c);
void del_n_copy(char *c, char* file);
#endif

int main(int argc, char **argv) {

    if (argc != 3) {
        printf("[ERROR] Not enough arguments\n");
        exit(-1);
    } else if (strlen(argv[1]) != 1) {
        printf("[ERROR] First argument must be of type char!\n");
    }
    else {
        del_n_copy(argv[1], argv[2]);
    }
}

#ifdef SYS
void del_n_copy(char* c, char* input) {

    int scr = open(input, O_RDONLY);
    if (scr < 0) {
        printf("[ERROR] cannot open file: %s \n", input);
        exit(-1);
    }
    int chars = 0;
    int lines = 0;

    int flag = 1;
    while (flag >= 0) {
        flag = is_line_empty(scr, c);
        chars += flag;
        if (flag > 0) {
            lines++;
        }
    }
    chars += 2;
    printf("chars = %d   lines = %d\n", chars, lines);
    close(scr);
}
int is_line_empty(int scr, char* c) {
    char* buffer = calloc(1, sizeof(char));

    int chars = 0;
    while (read(scr, buffer, sizeof(char)) == 1 && *buffer != '\n') {
        if (*c == *buffer) {
            chars++;
        }
    }
    if (read(scr, buffer, sizeof(char)) == 0 && chars == 0) {
        free(buffer);
        return -2;
    }

    free(buffer);
    lseek(scr, -1, SEEK_CUR);
    return chars;
}
#endif

#ifdef LIB
int is_line_empty(FILE* scr, char* c){
    char* buffer = calloc(1, sizeof(char));

    int chars = 0;
    while (fread(buffer, sizeof(char), 1, scr) == 1 && *buffer != '\n') {
        if (*buffer == *c) {
            chars++;
        }
    }
    if (fread(buffer, sizeof(char), 1, scr) == 0 && chars == 0) {
        // end of file
        free(buffer);
        return -2;
    }
    free(buffer);
    fseek(scr, -1, SEEK_CUR);
    return chars;
}

void del_n_copy(char *c, char* input) {
    FILE *scr = fopen(input, "r");
    if (scr == NULL) {
        printf("[ERROR] cannot open file: %s \n", input);
        exit(-1);
    }
    int chars = 0;
    int lines = 0;

    int flag = 1;
    while (flag >= 0) {
        flag = is_line_empty(scr, c);
//        printf("chars = %d, flag = %d\n", chars, flag);
        chars += flag;
        if (flag > 0) {
            lines++;
        }
    }
    chars += 2;
    printf("chars = %d   lines = %d\n", chars, lines);
    fclose(scr);
}
#endif
