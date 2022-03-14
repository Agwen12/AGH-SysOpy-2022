#include <stdio.h>
#include <malloc.h>
#include "stringlibrary.c"
#include "printlibrary.c"
int main() {
    int n = 10;
    float *p1;
    float *p2;
    p1 = (float*) malloc(n*sizeof(float));
    p2 = (float*) calloc(n, sizeof(float));
    printarray(p1);
    printarray(p2);
    free(p1);
    free(p2);
    return 0;
}
