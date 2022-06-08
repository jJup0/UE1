/*
 * rmerge.c
 *
 *  Created on: May 12, 2022
 *      Author: Jakob
 */

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "merge.h"

#define SCHEDULE_STRATEGY schedule(guided, 64)

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
    }
    return lo;
}

void merge(double a[], long n, double b[], long m, double c[]) {
#pragma omp parallel
    {
        int i;
        int j;

        // rank all elements in a, and insert them into c
#pragma omp for SCHEDULE_STRATEGY nowait
        for (i = 0; i < n; i++) {
            c[i + rank(a[i], b, m)] = a[i];
        }

        // rank all elements in b, and insert them into c
#pragma omp for SCHEDULE_STRATEGY nowait
        for (j = 0; j < m; j++) {
            c[j + rank(b[j], a, n)] = b[j];
        }
    }
}
/*
./bin/merge1_tester -n 10 -m 8 -p 2 -c
./bin/merge1_tester -n 10000000 -m 20000000 -p 16 -c
./bin/merge1_tester -n 100000000 -m 200000000 -p 2 -c
./bin/merge1_tester -n 100000000 -m 200000000 -s
*/