#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "merge.h"

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
            divAndConquerMergeSizeBalanceSwap(A, n, B, m, C, CUTOFF);
        }
    }
}
