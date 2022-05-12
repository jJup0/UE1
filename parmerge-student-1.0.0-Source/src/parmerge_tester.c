/*
 * parmerge_tester.c
 *
 *  Created on: Mar 12, 2019
 *      Author: sascha
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>

#include <omp.h>
#include "merge.h"

static int verbose = 0;

static int cmpfunc(const void * a, const void * b) {
  return (*(double*) a - *(double*) b);
}

int is_verbose() {
  return verbose;
}

void usage(const char *prog_name)
{
    fprintf(stderr, "usage: %s -n <int> -m <int> -p <int> [-r <int>] [-s] [-c] [-v]  -h\n"
        "-n number elements in first array\n"
        "-m number elements in second array\n"
        "-p number threads\n"
        "-s force sequential version\n"
        "-r number of repetitions\n"
        "-c check results\n"
        "-v verbose mode\n"
        "-h print this help message\n"
        , prog_name);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {

  int tok;
  long i;
  long n = 1, nset=0;
  long m = 1, mset=0;
  int p = 1, pset=0;
  int force_seq = 0;
  int nrep = 1;

  double *a, *b, *c;
  double time_start, time_end;
  int check = 0;
  int help = 0;

  opterr = 0;

  while ((tok = getopt(argc, argv, "n:m:p:r:svhc")) != -1) {
    switch (tok) {
    case 'n':
      n = atol(optarg);
      nset = 1;
      break;
    case 'm':
      m = atol(optarg);
      mset = 1;
      break;
    case 'p':
      p = atoi(optarg);
      pset = 1;
      break;
    case 'r':
      nrep = atoi(optarg);
      break;
    case 's':
      force_seq = 1;
      break;
    case 'v':
      verbose = 1;
      break;
    case 'h':
      help = 1;
      break;
    case 'c':
      check = 1;
      break;
    default:
      fprintf(stderr, "unknown parameter\n");
      exit(1);
    }
  }

  if( (nset + mset < 2) || help == 1 ) {
    usage(basename(argv[0]));
  }

  if( (pset + force_seq < 1) || help == 1 ) {
    usage(basename(argv[0]));
  }

  a = (double*)calloc(n, sizeof(double));
  b = (double*)calloc(m, sizeof(double));
  c = (double*)calloc(n+m, sizeof(double));


  omp_set_dynamic(0);
  omp_set_num_threads(p);

#pragma omp parallel for
  for(i=0; i<n; i++) {
    a[i] = 2*i+1;
  }

#pragma omp parallel for
  for(i=0; i<m; i++) {
    b[i] = 2*i;
  }

  if(is_verbose()) {
    printf("a:");
    for(i=0; i<n; i++) {
      printf(" %g", a[i]);
    }
    printf("\n");

    printf("b:");
    for(i=0; i<m; i++) {
      printf(" %g", b[i]);
    }
    printf("\n");
  }

  printf("p,n,m,time\n");
  for(i=0; i<nrep; i++) {
    time_start = omp_get_wtime();
    if( force_seq ) {
      seq_merge1(a, n, b, m, c);
      p = 1;
    } else {
      merge(a, n, b, m, c);
    }
    time_end = omp_get_wtime();
    printf("%d,%ld,%ld,%.6f\n", p, n, m, time_end-time_start);
  }

  if(is_verbose()) {
    printf("c:");
    for(i=0; i<n+m; i++) {
      printf(" %g", c[i]);
    }
    printf("\n");
  }

  if( check == 1 ) {
    int ok = 1;

    double *res = (double*)calloc(n+m, sizeof(double));

    // copy first array into res
    memcpy(res, a, n*sizeof(double));
    // copy second array into res
    memcpy(res+n, b, m*sizeof(double));

    // now sort this array
    qsort(res, n+m, sizeof(double), cmpfunc);

    if(is_verbose()) {
      printf("ref:");
      for(i=0; i<n+m; i++) {
        printf(" %g", res[i]);
      }
      printf("\n");
    }


    // now check elem per elem
    for(i=0; i<n+m; i++) {
      if( res[i] != c[i] ) {
        ok = 0;
        fprintf(stderr, "ERROR: position %ld should be %g but is %g\n", i, res[i], c[i]);
        break;
      }
    }

    if( ok == 1 ) {
      fprintf(stderr, "PASSED\n");
    }
  }


  free(a);
  free(b);
  free(c);

  return 0;
}

