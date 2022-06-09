#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "helper.h"

#define SCHEDULE_STRATEGY schedule(guided, 64)

int rankA(double val, double arr[], int n) {
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
int rankB(double val, double arr[], int n) {
    int lo = 0;
    int hi = n - 1;
    while (lo <= hi) {
        int mid = (lo + hi) >> 1;
        if (arr[mid] <= val) {
            lo = mid + 1;
        } else {
            hi = mid - 1;
        }
    }
    return lo;
}

void merge(double a[], long n, double b[], long m, double c[]) {
    // #pragma omp parallel
    {
        int i;
        int j;

        // rank all elements in a, and insert them into c
        // #pragma omp for SCHEDULE_STRATEGY nowait
        for (i = 0; i < n; i++) {
            int r = rankB(a[i], b, m);
            printf("rank a[i] in b = %d\n", r);
            c[i + r] = a[i];
        }

        // rank all elements in b, and insert them into c
        // #pragma omp for SCHEDULE_STRATEGY nowait
        for (j = 0; j < m; j++) {
            int r = rankA(b[j], a, n);
            printf("rank b[j] in a = %d\n", r);
            c[j + r] = b[j];
        }
    }
}

#define ARRAY_SIZE_N 20
#define ARRAY_SIZE_M 10

void main() {
    double a[ARRAY_SIZE_N];
    long n = ARRAY_SIZE_N;

    double b[ARRAY_SIZE_M];
    long m = ARRAY_SIZE_M;

    double c[ARRAY_SIZE_N + ARRAY_SIZE_M];
    for (int i = 0; i < n; i++) {
        a[i] = (i / 2) * 2;
    }
    for (int i = 0; i < m; i++) {
        b[i] = (i / 2) * 2;
    }

    merge(a, n, b, m, c);
    printf("adresses: %d, %d\n", &a[0], &b[0]);
    printDoubleArray(a, n);
    printDoubleArray(b, m);
    printDoubleArray(c, n + m);
}
/*
./bin/merge1_tester -n 10 -m 8 -p 2 -c
./bin/merge1_tester -n 10000000 -m 20000000 -p 16 -c
./bin/merge1_tester -n 100000000 -m 200000000 -p 2 -c
./bin/merge1_tester -n 100000000 -m 200000000 -s
*/