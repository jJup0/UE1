/**
 * @file merge2.c
 * @author Elias Pinter <12023962>, Kurdo-Jaroslav Asinger <01300351>, Jakob Roithinger <52009269> 
 * @brief Program merge2: This program takes a sorted array a with length n, a sorted array b with length and an array c with length n+m as input
 * and then merges the two arrays a and b into the array c by coranking.
 * @details The array C is partitioned into blocks of size (n+m)/t where t is the number of active threads. All coranks
 * for start and end of each block are computed in parallel. The coranking is performed sequentially in a bninary search like fashion. 
 **/

#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include "merge.h"

#define SCHEDULE_STRATEGY schedule(auto)


/**
 * @brief returns the minimum of a and b
 * @param a: int variable
 * @param b: int variable 
 */
int min(int a, int b) { 
    return (a <= b) ? a : b; 
}

/**
 * @brief returns the maximum a and b
 * @param a: int variable
 * @param b: int variable 
 */
int max(int a, int b) {
    return (a > b) ? a : b;
}

/**
 * @brief sets the coranks corank_a and corank_b for an index i in array c
 * @param i: index in array C for which the coranks are computed
 * @param A: sorted array
 * @param n: length of A
 * @param corank_a: points to the address where the corank of the array A is stored
 * @param B: sorted array
 * @param m: legnth of B
 * @param corank_b: points to the address where the corank of the array B is stored
 */
void corank(int i, double A[], long n,  int* corank_a, double B[], long m, int* corank_b) {
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

/**
 * @brief function that performs the merge.
 * @details array C is partitioned in blocks of size (n+m)/t where t is the number of threads. The coranks of start and end od those are
 * computed via a parallelized for loop. Then also the merging into those blocks is performed in a parallelized for loop
 * @param A: sorted array
 * @param n: length of A
 * @param corank_a points to the address where the corank of the array A is stored
 * @param B: sorted array
 * @param m: legnth of B
 * @param corank_b: points to the address where the corank of the array B is stored
 */
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
