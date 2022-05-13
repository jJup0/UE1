/*
 * parmerge.h
 *
 *  Created on: May 12, 2022
 *      Author: Elias
 */


#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include "merge.h"

typedef struct{
  long j;
  long k;
}Coranks;

void printDoubleArray(double* array, long n){
  fprintf(stderr, "[%f", array[0]);
  for(int i = 1; i < n; i++){
    fprintf(stderr, ", %f", array[i]);
  }
  fprintf(stderr, "]\n");
}

void printLongArray(long* array, long n){
  fprintf(stderr, "[%ld", array[0]);
  for(int i = 1; i < n; i++){
    fprintf(stderr, ", %ld", array[i]);
  }
  fprintf(stderr, "]\n");
}

long min(long a, long b){
  return (a <= b) ? a : b;
}

long max(long a, long b){
  return (a > b) ? a : b;
}

Coranks corank(long index, double a[] , long n, double b[], long m, double c[]){
  fprintf(stderr, "%s", "starting to calculate coranks ... \n\n");



  long j = min(index, n);
  // Because j + k = index 
  long k = index - j;
  long j_low = max(0, index-m);
  long k_low = 0;
  long delta;
  int all_lemmas_statisfied = 0;
  Coranks coranks = {0, 0};

  while(!all_lemmas_statisfied){
    if(j > 0 && k < m && a[j-1] > b[k]){
      delta = ceil((j - j_low)/2);
      k_low = k;
      j -= delta;
      k += delta;
    }
    else if(k > 0 && j < n && b[k-1] >= a[j]){
      delta = ceil((k-k_low)/2);
      j_low = j;
      j += delta;
      k -= delta;
    }
    else{
      fprintf(stderr, "j: %ld\n", j);
      fprintf(stderr, "k: %ld\n\n", k);
      coranks.j = j;
      coranks.k = k;
      all_lemmas_statisfied = 1;
    }
  }
  return coranks;
}

void merge(double a[], long n, double b[], long m, double c[]) {
  fprintf(stderr, "%s", "starting corank_merge ... \n\n");

  // printing array a;
  fprintf(stderr, "array a :");
  printDoubleArray(a, n);

  // printing array b;
  fprintf(stderr, "array b :");
  printDoubleArray(b, m);

  // printing array b;
  fprintf(stderr, "array c :");
  printDoubleArray(c, n+m);


#pragma omp parallel
  {
    int t = omp_get_num_threads();
    long coranks_a [t+1];
    long coranks_b [t+1];

    fprintf(stderr, "%s", "calculating coranks ... \n\n");
#pragma omp for nowait
    for(int i = 0; i < t; i++){
      Coranks coranks = corank(i*(n+m)/t, a, n, b, m, c);
      coranks_a[i] = coranks.j;
      coranks_b[i] = coranks.k; 
    }
    coranks_a[t] = n;
    coranks_b[t] = m;
    fprintf(stderr, "%s", "calculated coranks ... \n\n");




    fprintf(stderr, "%s", "merging block from a and b into corresponding block in c ... \n\n");
#pragma omp for nowait
    for(int i = 0; i < t; i++){
      // printing array coranks_a;
      fprintf(stderr, "array coranks_a :");
      printLongArray(coranks_a, t+1);

      // printing array coranks_b;
      fprintf(stderr, "array coranks_b :");
      printLongArray(coranks_b, t+1);
      long lower_a = coranks_a[i];
      long upper_a = coranks_a[i+1];
      double* block_a = &a[lower_a];
      long block_a_length = upper_a - lower_a;
      long lower_b = coranks_b[i];
      long upper_b = coranks_b[i+1];
      double* block_b = &b[lower_b];
      long block_b_length = upper_b - lower_b;
      double* block_c = &c[i*(n+m)/t];
      fprintf(stderr, "starting sequential merge with block_a_length: %ld,  block_b_length: %ld\n\n", block_a_length, block_b_length);
      seq_merge1(block_a, block_a_length, block_b, block_b_length, block_c);
    }
  }
}


