/*
 * rmerge.c
 *
 *  Created on: May 12, 2022
 *      Author: Jakob
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include "merge.h"

int rank ( double val , double arr [] , int n ){
  int lo = 0;
  int hi = n-1;
  while (lo <= hi){
    int mid = (lo + hi) >> 1;
    if (arr[mid] < val){
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
#pragma omp for nowait
    for (i=0; i<n; i++) {
      int rank_a = rank(a[i], b, m);
      c[i+rank_a] = a[i];
    }

    // rank all elements in b, and insert them into c
#pragma omp for nowait
    for (j=0; j<m; j++) {
      int rank_b = rank(b[j], a, n);
      c[j+rank_b] = b[j];
    }
  }
}
/*
./bin/merge1_tester -n 10 -m 8 -p 2 -c
*/