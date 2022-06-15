/**
 * @file merge3.c
 * @author Elias Pinter <12023962>, Kurdo-Jaroslav Asinger <01300351>, Jakob Roithinger <52009269> 
 * @brief Program merge3: This program takes a sorted array a with length n, a sorted array b with length and an array c with length n+m as input
 * and then merges the two arrays a and b into the array c by coranking.
 * @details The array C is partitioned into blocks of size (n+m)/t where t is the number of active threads. All coranks
 * for start and end of each block are computed in parallel. The coranking is performed sequentially in a bninary search like fashion. 
 **/

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include "merge.h"

/**
 * @brief This function returns the rank of an element val in the array arr with length n.
 * @param val element that is being ranked
 * @param Arr array that is being ranked into
 * @param n length of the array A
 */
int rank(double val, double Arr[], int n) {
    int lo = 0;
    int hi = n - 1;
    while (lo <= hi) {
        int mid = (lo + hi) >> 1;
        if (Arr[mid] < val) {
            lo = mid + 1;
        } else {
            hi = mid - 1;
        }
        // fprintf(stderr, "binsearch\n");
    }
    return lo;
}

/**
 * @brief function that performs the merge.
 * @details If one of the arrays A or B is already fully merged copying is performed using parallelized loops.
 * If not it is checked if the problem size is already smaller than the cutoff value, then merging is performed sequentially.
 * If this is not the case it is first checked if A is the bigger array, if not the arrays are swapped. After that the element in the middle of
 * A is ranked and inserted into C in the correct psoition, followed with two recursive calls: one with the lower half and one with the upper half
 * of the A and C array
 * @param A: sorted array
 * @param n: length of A
 * @param B: sorted array
 * @param m: legnth of B
 * @param C: array a and b are being merged into
 * @param CUTOFF: cutoff value to stop recursion
 */
void divAndConquerMergeSizeBalanceSwap(double A[], long n, double B[], long m, double C[], const long CUTOFF) {
    int i;
    if (n == 0) {
#pragma omp taskloop default(shared|none) shared(A, n, B, m, C, CUTOFF, i)
        for (i = 0; i < m; i++) {
            C[i] = B[i];
        }
    } else if (m == 0) {
#pragma omp taskloop default(shared|none) shared(A, n, B, m, C, CUTOFF, i)
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
#pragma omp task default(none) (shared|none) shared(A, B, C) firstprivate(r, s)
        { divAndConquerMergeSizeBalanceSwap(A, r, B, s, C, CUTOFF); }
#pragma omp task default(none) (shared|none) shared(A, B, C) firstprivate(r, s, m, n)
        { divAndConquerMergeSizeBalanceSwap(&A[r + 1], n - r - 1, &B[s], m - s, &C[r + s + 1], CUTOFF); }
    }
}

void merge(double A[], long n, double B[], long m, double C[]) {
#pragma omp parallel default(shared|none) shared (A, n, B, m, C)
    {
#pragma omp master
        {
            long CUTOFF = (m + n) / (omp_get_num_threads());
            divAndConquerMergeSizeBalanceSwap(A, n, B, m, C, CUTOFF);
        }
    }
}
