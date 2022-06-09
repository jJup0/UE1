#include <stdio.h>

void printIntArray(int* array, int n) {
    fprintf(stderr, "[%d", array[0]);
    for (int i = 1; i < n; i++) {
        fprintf(stderr, ", %d", array[i]);
    }
    fprintf(stderr, "]\n");
}

void printDoubleArray(double* array, int n) {
    fprintf(stderr, "[%.0f", array[0]);
    for (int i = 1; i < n; i++) {
        fprintf(stderr, ", %.0f", array[i]);
    }
    fprintf(stderr, "]\n");
}