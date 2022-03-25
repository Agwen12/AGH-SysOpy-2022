
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>


#ifdef LIB
int is_line_empty(FILE* scr);
void del_n_copy(char* input, char* output);
#endif

#ifdef SYS
#include <fcntl.h>
#include <unistd.h>
int is_line_empty(int scr);
void del_n_copy(char* input, char* output);
#endif

int main(int argc, char **argv) {
    char* arguments[2];
    if (argc <= 1) {
        printf("[ERROR] Not enough arguments\n");
    }
    if (argc == 3) {
        del_n_copy(argv[1], argv[2]);
    } else {

        printf("give arguments:\n" );

        char* buffer = calloc(sizeof(char), 777);
        scanf("%10s", buffer);
        arguments[0] = calloc(strlen(buffer), sizeof(char));
        strcat(arguments[0], buffer);
        // free(buffer);
        buffer[0] = '\0';

        // char* buffer1 = calloc(sizeof(char), 777);
        scanf("%10s", buffer);
        arguments[1] = calloc(strlen(buffer), sizeof(char));
        strcat(arguments[1], buffer);
        free(buffer);

        // printf("%s\n%s\n", arguments[0], arguments[1]);
        del_n_copy(arguments[0], arguments[1]);

    }
    // printf("[KONIEC PROGRAMU]\n" );


}

#ifdef SYS
int is_line_empty(int scr) {
    char* buffer = calloc(1, sizeof(char));

    int current_position = lseek(scr, 0L, SEEK_CUR);
    int line_len = 0;
    int spaces = 0;
    while (read(scr, buffer, sizeof(char)) == 1 && *buffer != '\n') {
        // printf("%s\n",buffer);
        line_len++;
        if (isspace(*buffer)) {
            spaces++;
        }
    }
    if (read(scr, buffer, sizeof(char)) == 0 && line_len == 0) {
        // printf("[END OF FILE]\n");
        free(buffer);
        return 0;
    }

    if (spaces == line_len) {
        free(buffer);
        // printf("[EPMTY]\n" );
        lseek(scr, -1, SEEK_CUR);
        return -1;
    }
    // printf("[NOT EMPTY]\n");
    lseek(scr, current_position, SEEK_SET);
    free(buffer);
    return line_len;
}

void del_n_copy(char* input, char* output) {
    int flag = 1;
    int scr = open(input, O_RDONLY);
    int out = open(output, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);


    while (flag != 0) {
        flag = is_line_empty(scr);
        // printf("%d\n", flag);
        if (flag != -1) {
            // printf("[FLAG] %d\t", flag);
            char* buffer = calloc(flag + 1, sizeof(char));
            read(scr, buffer, flag + 1);
            // printf("[BUFFER]   %s", buffer);
            write(out, buffer, flag + 1);
            free(buffer);
        }
    }
    close(scr);
    close(out);
}
#endif

#ifdef LIB
int is_line_empty(FILE* scr){
    char* buffer = calloc(1, sizeof(char));

    int current_position = ftell(scr);
    int line_len = 0;
    int spaces = 0;
    while (fread(buffer, sizeof(char), 1, scr) == 1 && *buffer != '\n') {
        line_len++;
        if (isspace(*buffer)) {
            spaces++;
        }
    }
    if (fread(buffer, sizeof(char), 1, scr) == 0 && line_len == 0) {
        // printf("[END OF FILE]\n");
        free(buffer);
        return 0;
    }
    if (spaces == line_len) {
        free(buffer);
        // printf("[EMPTY]\n
        fseek(scr, -1, SEEK_CUR);
        return -1;
    }
    fseek(scr, current_position, SEEK_SET);
    // printf("[NOT EMPTY]\n" );
    free(buffer);
    return line_len;
}


void del_n_copy(char* input, char* output) {
    int flag = 1;
    FILE *scr;
    scr = fopen(input, "r");

    FILE* out = fopen(output, "w+");
    while (flag != 0) {
        flag = is_line_empty(scr);
        if (flag != -1) {
            // printf("[FLAG] %d\t", flag);
            char* buffer = calloc(flag + 1, sizeof(char));
            fread(buffer, sizeof(char), flag + 1, scr);
            // printf("[BUFFER]   %s", buffer);
            fwrite(buffer, sizeof(char), flag + 1, out);
            free(buffer);
        }
    }
    fclose(scr);
    fclose(out);
}
#endif
