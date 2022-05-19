#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "merge.h"


int rank (double val , double arr [] , int n ){
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


void merge(double A[], long n, double B[], long m, double C[]) {
  int i;
  #pragma omp parallel
    {
      omp_set_nested(1);
      omp_set_max_active_levels(5);
      int CUTOFF = omp_get_num_threads()*2;
      // fprintf(stderr, "cuoff: %d\n", CUTOFF);
      if (n == 0){
        #pragma omp for
        for(i = 0; i < m; i++){
          C[i] = B[i];
        }
      }
      else if(m == 0){
        #pragma omp for
        for(i = 0; i < n; i++){
          C[i] = A[i];
        }
      }
      else if(n+m < CUTOFF){
        omp_set_nested(0);
        seq_merge1(A, n, B, m, C);
      }
      else{
        int r = n/2;
        int s = rank(A[r], B, m);
        C[r+s] = A[r];
        #pragma omp single nowait
          {
            #pragma omp task shared(r, s)
              {
                fprintf(stderr, "thread executing this task: %d\n", omp_get_thread_num());
                merge(A, r, B, s, C);
              }
            #pragma omp task shared(r, s)
              {
                fprintf(stderr, "thread executing this task: %d\n", omp_get_thread_num());
                merge(&A[r+1], n-r-1, &B[s], m-s, &C[r+s+1]);
              }
          }
      }
    }
}