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

void divAndConquerMergeSizeBalanceBranch(double A[], long n, double B[], long m, double C[], const long CUTOFF) {
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
        int s, r, s_inc, r_inc;
        if (n >= m) {
            r = n >> 1;
            s_inc = s = rank(A[r], B, m);
            C[r + s] = A[r];
            r_inc = r + 1;
        } else {
            s = m >> 1;
            r_inc = r = rank(B[s], A, n);
            C[s + r] = B[s];
            s_inc = s + 1;
        }
#pragma omp task shared(A, B, C)
        { divAndConquerMergeSizeBalanceBranch(A, r, B, s, C, CUTOFF); }
#pragma omp task shared(A, B, C)
        { divAndConquerMergeSizeBalanceBranch(&A[r_inc], n - r_inc, &B[s_inc], m - s_inc, &C[r + s + 1], CUTOFF); }
    }
}

void divAndConquerMergeSizeBalanceSwap(double A[], long n, double B[], long m, double C[], const long CUTOFF) {
    int i;
    if (n == 0) {
#pragma omp taskloop
        for (i = 0; i < m; i++) {
            C[i] = B[i];
        }
    } else if (m == 0) {
#pragma omp taskloop
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
        int r = n >> 1;
        int s = rank(A[r], B, m);
        C[r + s] = A[r];
#pragma omp task default(none) shared(A, B, C) firstprivate(r, s)
        { divAndConquerMergeSizeBalanceSwap(A, r, B, s, C, CUTOFF); }
#pragma omp task default(none) shared(A, B, C) firstprivate(r, s, m, n)
        { divAndConquerMergeSizeBalanceSwap(&A[r + 1], n - r - 1, &B[s], m - s, &C[r + s + 1], CUTOFF); }
    }
}

void merge(double A[], long n, double B[], long m, double C[]) {
#pragma omp parallel
    {
#pragma omp master
        {
            long CUTOFF = (m + n) / (omp_get_num_threads());  // each thread should perform approximately 3 seq merges
            // divAndConquerMergeSizeBalanceBranch(A, n, B, m, C, CUTOFF);
            divAndConquerMergeSizeBalanceSwap(A, n, B, m, C, CUTOFF);
        }
    }
}
/*
 ./bin/merge3_tester -n 100 -m 80 -p 4 -c
 ./bin/merge3_tester -n 10000000 -m 20000000 -p 8 -c
 ./bin/merge3_tester -n 20000000 -m 10000 -p 8 -c
 ./bin/merge3_tester -n 10000 -m 20000000 -p 8 -c
*/