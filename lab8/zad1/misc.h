//
// Created by agwen on 21.05.22.
//

#ifndef LAB8_MISC_H
#define LAB8_MISC_H

#define ROW_LENGTH 256

#define TRY(a, b) do {               \
    if ((a) == -1) {                 \
        printf("[ERROR] %s", b);     \
        exit(1);                     \
    }                                \
} while(0)

#define INFO(info_message) do {    \
    printf("[INFO] %s", info_message);                                \
}


#define TRY_VAL(exp, error_val, error_message) do {      \
    if ((exp) == error_val) {                            \
        printf("[ERROR] %s\n", error_message);           \
        exit(1);                                         \
    }                                                    \
} while(0)


#endif //LAB8_MISC_H
