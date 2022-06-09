#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "helper.h"

#define ARRAY_SIZE 1000

void main() {
    int a[ARRAY_SIZE];
    int b[ARRAY_SIZE];
    int n = ARRAY_SIZE;
    for (int i = 0; i < ARRAY_SIZE; i++) {
        a[i] = rand() % (ARRAY_SIZE / 2);
    }
    printIntArray(a, ARRAY_SIZE);

    // 2.3 code:

    // assume a, b initialized arrays, each with a capacity of n integers
#pragma omp parallel default(none) shared(a, b) firstprivate(n)
    {
        int i;
#pragma omp for
        for (i = 0; i < n; i++) {
            int count = 0;
            int j;
            for (j = 0; j < i; j++) {
                if (a[j] <= a[i]) count++;
            }
            j++;
            for (; j < n; j++) {
                if (a[j] < a[i]) count++;
            }
            b[count] = a[i];
        }
        // implicit barrier
#pragma omp for
        for (i = 0; i < n; i++) {
            a[i] = b[i];
        }
    }
    printIntArray(a, ARRAY_SIZE);
}

// gcc -o 2.3 -fopenmp 2.3.c