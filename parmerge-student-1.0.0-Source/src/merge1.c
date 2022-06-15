/**
 * @file merge1.c
 * @author Elias Pinter <12023962>, Kurdo-Jaroslav Asinger <01300351>, Jakob Roithinger <52009269>
 * @brief Program merge1: This program takes a sorted array a with length n, a sorted array b with length and an array c with length n+m as input
 * and then merges the two arrays a and b into the array c by ranking the elements of a and b.
 * @details The ranking of the elements of array a into array b and vice versa are done in parallel as ranking two different elements is independent. The ranking
 * itself is implemented sequentially in a binary search like fashion. Every element is immediately inserted into c after it's rank was determined.
 **/

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include "merge.h"

// schedule strategy for prallel for loop when ranking
#define SCHEDULE_STRATEGY schedule(guided, 64)

/**
 * @brief This function returns the rank of an element val in the array arr with length n.
 * @param val element that is being ranked
 * @param A array that is being ranked into
 * @param n length of the array arr
 */
int rank(double val, double A[], int n) {
    int lo = 0;
    int hi = n - 1;
    while (lo <= hi) {
        int mid = (lo + hi) >> 1;
        if (A[mid] < val) {
            lo = mid + 1;
        } else {
            hi = mid - 1;
        }
    }
    return lo;
}

/**
 * @brief function that performs the merge.
 * @details ranks and inserts all elements of a into c via parallelized loop, then does the same for elements of b
 * @param A sorted array
 * @param n length of a
 * @param B sorted array
 * @param m length of b
 * @param C array a and b are being merged into
 */
void merge(double A[], long n, double B[], long m, double C[]) {
#pragma omp parallel default(none) shared(A, n, B, m, C)
    {
        int i;
        int j;

        // rank all elements in a, and insert them into c
#pragma omp for SCHEDULE_STRATEGY nowait
        for (i = 0; i < n; i++) {
            C[i + rank(A[i], B, m)] = A[i];
        }

        // rank all elements in b, and insert them into c
#pragma omp for SCHEDULE_STRATEGY nowait
        for (j = 0; j < m; j++) {
            C[j + rank(B[j], A, n)] = B[j];
        }
    }
}
