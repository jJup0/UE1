#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "merge.h"

#define SCHEDULE_STRATEGY schedule(guided, 64)

int rank(double val, double A[], int n) {
    int lo = 0;
    int hi = n - 1;
    while (lo <= hi) {
        int mid = (lo + hi) >> 1;
        if (A[mid] < val) {
            lo = mid + 1;
        } else {
            hi = mid - 1;
        }
    }
    return lo;
}

void merge(double A[], long n, double B[], long m, double C[]) {
#pragma omp parallel default(shared|none) shared(A, n, B, C)
    {
        int i;
        int j;

        // rank all elements in a, and insert them into c
#pragma omp for SCHEDULE_STRATEGY nowait
        for (i = 0; i < n; i++) {
            C[i + rank(A[i], B, m)] = A[i];
        }

        // rank all elements in b, and insert them into c
#pragma omp for SCHEDULE_STRATEGY nowait
        for (j = 0; j < m; j++) {
            C[j + rank(B[j], A, n)] = B[j];
        }
    }
}
