//
// Created by agwen on 11.03.2022.
//

#include <stdio.h>
void printarray(float *arr) {
    int length = sizeof(*arr)/sizeof(float);
    for (int i = 0; i < length; i++) {
        printf("%.6f\n", arr[i]);
    }
}
