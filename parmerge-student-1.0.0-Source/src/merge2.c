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

int min(int A, int b) { return (A <= b) ? A : b; }

int max(int A, int b) { return (A > b) ? A : b; }

void corank(int i, double A[], long m, int* corank_a, double B[], long n, int* corank_b) {
    int j = min(i, m);
    int k = i - j;  // Because j + k = i
    int j_low = max(0, i - n);
    int k_low = 0;  // uninit in pseudo code
    int delta;
    int active = 20;

    while (active > 0) {
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
            return  ;
        }
        active--;
        fprintf(stderr, "active: %d, j_low: %d, k_low: %d || B[k-1], B[k] = [%.1f, %.1f] || A[j-1], A[j] = [%.1f, %.1f]  \n", active, j_low, k_low, B[k - 1], B[k], A[j - 1], A[j]);
    }
}

void merge(double A[], long n, double B[], long m, double C[]) {
    int t = 10;
    int coj[t + 1];
    int cok[t + 1];
    int i;
    for (i = 0; i < t; i++) {
        corank(i * (n + m) / t, A, n, &coj[i], B, m, &cok[i]);
    }
    coj[t] = n;
    cok[t] = m;
}

void merge1(double A[], long n, double B[], long m, double C[]) {
    int coj[omp_get_max_threads() + 1];
    int cok[omp_get_max_threads() + 1];
#pragma omp parallel
    {
        int t = omp_get_num_threads();
        int i;

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
 ./bin/merge2_tester -n 100 -m 80 -p 4 -c
*/
