#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "merge.h"

void printIntArray(int* array, int n) {
    // fprintf(stderr, "[%d", array[0]);
    for (int i = 1; i < n; i++) {
        // fprintf(stderr, ", %d", array[i]);
    }
    // fprintf(stderr, "]\n");
}

void printDoubleArray(double* array, int n) {
    // fprintf(stderr, "[%.0f", array[0]);
    for (int i = 1; i < n; i++) {
        // fprintf(stderr, ", %.0f", array[i]);
    }
    // fprintf(stderr, "]\n");
}

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
        // fprintf(stderr, "binsearch\n");
    }
    return lo;
}

void divAndConquerMerge(double A[], long n, double B[], long m, double C[], int workDone[]) {
    // fprintf(stderr, "divandconq: n = %ld, m = %ld\n", n, m);

    int i;
    if (n == 0) {
#pragma omp parallel for
        for (i = 0; i < m; i++) {
            C[i] = B[i];
        }
    } else if (m == 0) {
#pragma omp parallel for
        for (i = 0; i < n; i++) {
            C[i] = A[i];
        }
    } else if (n + m < 100) {
        // fprintf(stderr, "going in seq merge\n");
        seq_merge1(A, n, B, m, C);
        // fprintf(stderr, "thread [%d] performed seq merge with n = %ld, m =%ld\n", omp_get_thread_num(), n, m);
#pragma omp critical
        { workDone[omp_get_thread_num()] = workDone[omp_get_thread_num()] + m + n; }
        // fprintf(stderr, "out of seq merge\n");
    } else {
        int r = n / 2;
        int s = rank(A[r], B, m);
        C[r + s] = A[r];
#pragma omp task shared(A, B, C, workDone)
        {
            // fprintf(stderr, "thread executing this task: %d\n", omp_get_thread_num());
            divAndConquerMerge(A, r, B, s, C, workDone);
        }
#pragma omp task shared(A, B, C, workDone)
        {
            // fprintf(stderr, "thread executing this task: %d\n", omp_get_thread_num());
            divAndConquerMerge(&A[r + 1], n - r - 1, &B[s], m - s, &C[r + s + 1], workDone);
        }
    }
}

void merge(double A[], long n, double B[], long m, double C[]) {
    // fprintf(stderr, "merge: n = %ld, m = %ld\n", n, m);

    int workDone[omp_get_max_threads()];
    for (int i = 0; i < omp_get_max_threads(); i++) {
        workDone[i] = 0;
    }
#pragma omp parallel
    {
#pragma omp single
        {
            omp_set_nested(1);
            divAndConquerMerge(A, n, B, m, C, workDone);
            // fprintf(stderr, "done\n");
        }
    }
    printIntArray(workDone, omp_get_max_threads());
    // printDoubleArray(C, m + n);
}
/*
 ./bin/merge3_tester -n 100 -m 80 -p 4 -c
 ./bin/merge3_tester -n 100 -m 80 -p 2 -c
*/