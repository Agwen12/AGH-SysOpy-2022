//
// Created by agwen on 21.04.2022.
//
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 1024

int main(int argc, char** argv) {


    if (argc == 2) {
        if (!strcmp("nadawca", argv[1])) {
            FILE* f = popen("mail -H | tail | sort -k 3", "w");
            pclose(f);
        } else if (!strcmp("data", argv[1])) {
            FILE* out = popen("mail -H | tail | sort -d", "w");
            pclose(out);
        } else {
            printf("[ERROR] EEEEEEERRRRRRRRRROOORRRRRR\n");
            return -1;
        }
    }  else if (argc == 4) {
        char buffer[4 * BUFFER_SIZE] = "mail -s '";
        strcat(
                strcat(
                        strcat(
                                buffer, argv[2]
                                ), "' "
                            ), argv[1]
                        );
        FILE* f = popen(buffer, "w");
        fputs("'", f);
        fputs(argv[3], f);
        fputs("'", f);
        pclose(f);
    } else {
        printf("[ERROR] Invalid argument\n");
        return -1;
    }
    return 0;
}