/*
 * parmerge.h
 *
 *  Created on: May 12, 2022
 *      Author: Elias
 */

#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "merge.h"

void printLongArray(long* array, long n) {
    fprintf(stderr, "[%ld", array[0]);
    for (long i = 1; i < n; i++) {
        fprintf(stderr, ", %ld", array[i]);
    }
    fprintf(stderr, "]\n");
}

void printIntArray(int* array, int n) {
    fprintf(stderr, "[%d", array[0]);
    for (int i = 1; i < n; i++) {
        fprintf(stderr, ", %d", array[i]);
    }
    fprintf(stderr, "]\n");
}

typedef struct {
    int j;
    int k;
} Coranks;

int min(int A, int b) { return (A <= b) ? A : b; }

int max(int A, int b) { return (A > b) ? A : b; }

void corank(int i, double A[], long m, int* corank_a, double B[], long n, int* corank_b) {
    int j = min(i, m);
    // Because j + k = i
    int k = i - j;
    int j_low = max(0, i - n);
    int k_low = 0;  // uninit in pseudo code
    int delta;
    int active = 0;

    while (!active) {
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
            fprintf(stderr, "j: %d, k: %d\n\n", j, k);
            *corank_a = j;
            *corank_b = k;
            // fprintf(stderr, "corank_a: %d, corank_b: %d\n", *corank_a, *corank_b);
            // fprintf(stderr, "ADRESSES: corank_a: %p, corank_b: %p\n", corank_a, corank_b);

            active = 1;
        }
    }
}

void merge(double A[], long n, double B[], long m, double C[]) {
#pragma omp parallel
    {
        int t = omp_get_num_threads();
        int i;
        int coj[t + 1];
        int cok[t + 1];
#pragma omp for
        for (i = 0; i < t; i++) {
            corank(i * (n + m) / t, A, n, &coj[i], B, m, &cok[i]);
        }
        coj[t] = n;
        cok[t] = m;
#pragma omp for
        for (i = 0; i < t; i++) {
            seq_merge1(&A[coj[i]], coj[i + 1] - coj[i], &B[cok[i]], cok[i + 1] - cok[i], &C[i * (n + m) / t]);
        }
    }
}

/*
 ./bin/merge2_tester -n 10 -m 8 -p 2 -c
*/

//     int arr[10];
// #pragma omp parallel
//     {
//         int i;
// #pragma omp for
//         for (i = 0; i < 100; i++) {
//             arr[i] = 0;
//         }
// #pragma omp master
//         { fprintf(stderr, "arr[0]: %d\n", arr[0]); }
//     }

void mergebroken(double A[], long n, double B[], long m, double C[]) {
    int* coranks_a;
    int* coranks_b;
#pragma omp parallel shared(coranks_a, coranks_b)
    {
        int t = omp_get_num_threads();
#pragma omp master
        {
            coranks_a = malloc((t + 1) * (sizeof(int)));
            coranks_b = malloc((t + 1) * (sizeof(int)));
        }

        int i;

#pragma omp for
        for (i = 0; i < t; i++) {
            // corank(i * (n + m) / t, A, n, &coranks_a[i], B, m, &coranks_b[i]);
            // fprintf(stderr, "ADRESSES: corank_a: %p, corank_b: %p\n\n", &coranks_a[i], &coranks_b[i]);
            fprintf(stderr, "calc corank for i=%d, corank_a = %d, corank_b = %d\n", i, coranks_a[i], coranks_b[i]);
        }

#pragma omp master
        {
            coranks_a[t] = n;
            coranks_b[t] = m;
            fprintf(stderr, "single update: \n");
            printIntArray(coranks_a, t + 1);
            printIntArray(coranks_b, t + 1);
        }

#pragma omp for
        for (i = 0; i < t; i++) {
            // fprintf(stderr, "length A: %ld, length B: %ld\n", coranks_a[i + 1] - coranks_a[i], coranks_b[i + 1] - coranks_b[i]);
            seq_merge1(&A[coranks_a[i]], coranks_a[i + 1] - coranks_a[i], &B[coranks_b[i]], coranks_b[i + 1] - coranks_b[i], &C[i * (n + m) / t]);
        }
    }
}