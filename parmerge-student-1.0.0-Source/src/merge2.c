#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "merge.h"

#define SCHEDULE_STRATEGY schedule(auto)

int min(int A, int b) { return (A <= b) ? A : b; }

int max(int A, int b) { return (A > b) ? A : b; }

void corank(int i, double A[], long m, int* corank_a, double B[], long n, int* corank_b) {
    int j = min(i, m);
    int k = i - j;  // Because j + k = i
    int j_low = max(0, i - n);
    int k_low = 0;  // uninit in pseudo code
    int delta;

    while (1) {
        if (j > 0 && k < n && A[j - 1] > B[k]) {
            delta = ceil((j - j_low) / 2);
            k_low = k;
            j -= delta;
            k += delta;
        } else if (k > 0 && j < m && B[k - 1] >= A[j]) {
            delta = ceil((k - k_low) / 2);
            j_low = j;
            j += delta;
            k -= delta;
        } else {
            *corank_a = j;
            *corank_b = k;
            return;
        }
    }
}

void merge(double A[], long n, double B[], long m, double C[]) {
    int coj[omp_get_max_threads() + 1];
    int cok[omp_get_max_threads() + 1];
#pragma omp parallel default(shared|none) shared(A, n, B, m, C, coj, cok)
    {
        int t = omp_get_num_threads();
        int i;
#pragma omp single nowait
        {
            coj[t] = n;
            cok[t] = m;
        }
#pragma omp for SCHEDULE_STRATEGY
        for (i = 0; i < t; i++) {
            corank(i * (n + m) / t, A, n, &coj[i], B, m, &cok[i]);
        }

#pragma omp for SCHEDULE_STRATEGY
        for (i = 0; i < t; i++) {
            seq_merge1(&A[coj[i]], coj[i + 1] - coj[i], &B[cok[i]], cok[i + 1] - cok[i], &C[i * (n + m) / t]);
        }
    }
}
