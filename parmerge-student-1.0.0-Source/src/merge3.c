#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "merge.h"

#define debug fprintf

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

void divAndConquerMerge(double A[], long n, double B[], long m, double C[], const long CUTOFF) {
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
    } else if (n + m < CUTOFF) {
        seq_merge1(A, n, B, m, C);
    } else {
        if (n < m) {
            double* temp_arr = A;
            A = B;
            B = temp_arr;
            long temp_len = n;
            n = m;
            m = temp_len;
        }
        int r = n / 2;
        int s = rank(A[r], B, m);
        C[r + s] = A[r];
#pragma omp task shared(A, B, C)
        { divAndConquerMerge(A, r, B, s, C, CUTOFF); }
#pragma omp task shared(A, B, C)
        { divAndConquerMerge(&A[r + 1], n - r - 1, &B[s], m - s, &C[r + s + 1], CUTOFF); }
    }
}

void divAndConquerMergeDebug(double A[], long n, double B[], long m, double C[], int seq_work_done[]) {
    // debug(stderr, "divandconq: n = %ld, m = %ld\n", n, m);

    int i;
    if (n == 0) {
#pragma omp for
        for (i = 0; i < m; i++) {
            C[i] = B[i];
        }
    } else if (m == 0) {
#pragma omp for
        for (i = 0; i < n; i++) {
            C[i] = A[i];
        }
    } else if (n + m < 100) {
        // debug(stderr, "going in seq merge\n");
        seq_merge1(A, n, B, m, C);
        // debug(stderr, "thread [%d] performed seq merge with n = %ld, m =%ld\n", omp_get_thread_num(), n, m);
#pragma omp critical
        { seq_work_done[omp_get_thread_num()] = seq_work_done[omp_get_thread_num()] + m + n; }
        // debug(stderr, "out of seq merge\n");
    } else {
        int r = n / 2;
        int s = rank(A[r], B, m);
        C[r + s] = A[r];
#pragma omp task shared(A, B, C, seq_work_done)
        {
            // debug(stderr, "thread executing this task: %d\n", omp_get_thread_num());
            divAndConquerMergeDebug(A, r, B, s, C, seq_work_done);
        }
        // #pragma omp task shared(A, B, C, seq_work_done)
        //         {
        // debug(stderr, "thread executing this task: %d\n", omp_get_thread_num());
        divAndConquerMergeDebug(&A[r + 1], n - r - 1, &B[s], m - s, &C[r + s + 1], seq_work_done);
        // }
    }
}

void merge_debug(double A[], long n, double B[], long m, double C[]) {
    // debug(stderr, "merge: n = %ld, m = %ld\n", n, m);
    int seq_work_done[omp_get_max_threads()];
    for (int i = 0; i < omp_get_max_threads(); i++) seq_work_done[i] = 0;
#pragma omp parallel
    {
#pragma omp single
        {
            omp_set_nested(1);
            divAndConquerMergeDebug(A, n, B, m, C, seq_work_done);
        }
    }
    printIntArray(seq_work_done, omp_get_max_threads());
}

void merge(double A[], long n, double B[], long m, double C[]) {
    if (0) {
        fprintf(stderr, "DEBUG VERSION CALLED\n");
        merge_debug(A, n, B, m, C);
    } else {
#pragma omp parallel
        {
#pragma omp master
            {
                // omp_set_nested(1);                                    // to allow parallel for in divAndConquerMerge
                long CUTOFF = (m + n) / (3 * omp_get_max_threads());  // each thread should perform approximately 3 seq merges
                divAndConquerMerge(A, n, B, m, C, CUTOFF);
            }
        }
    }
}
/*
 ../bin/merge3_tester -n 100 -m 80 -p 4 -c
 ./bin/merge3_tester -n 10000000 -m 20000000 -p 8 -c
 ./bin/merge3_tester -n 20000000 -m 10000 -p 8 -c
 ./bin/merge3_tester -n 10000 -m 20000000 -p 8 -c
*/