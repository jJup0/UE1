#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "merge.h"

#define SCHEDULE_STRATEGY schedule(guided, 64)

int rank(double val, double arr[], int n) {
    int lo = 0;
    int hi = n - 1;
    while (lo <= hi) {
        int mid = (lo + hi) >> 1;
        if (arr[mid] < val) {
            lo = mid + 1;
        } else {
            hi = mid - 1;
        }
    }
    return lo;
}

void merge(double a[], long n, double b[], long m, double c[]) {
#pragma omp parallel default(shared|none) shared(a, n, b, c)
    {
        int i;
        int j;

        // rank all elements in a, and insert them into c
#pragma omp for SCHEDULE_STRATEGY nowait
        for (i = 0; i < n; i++) {
            c[i + rank(a[i], b, m)] = a[i];
        }

        // rank all elements in b, and insert them into c
#pragma omp for SCHEDULE_STRATEGY nowait
        for (j = 0; j < m; j++) {
            c[j + rank(b[j], a, n)] = b[j];
        }
    }
}
