#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <assert.h>
#include <math.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>

#include "utils.h"
#include "mv_instance.h"
#include "mv.h"

int verbose = 0;
//MPI_Datatype mpi_datatype = MPI_INT;
MPI_Datatype mpi_datatype = MPI_DOUBLE;

int is_verbose() {
  return verbose;
}

void usage(const char *prog_name) {
  fprintf(stderr, "usage: %s -n <int> [-v] [-c] [-h]\n"
                  "-n number elements per of local matrix (nxn)\n"
                  "-v verbose mode\n"
                  "-c check results\n"
                  "-h print this help message\n", prog_name);
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  int rank, size;

  int n = 0;     // number of elements in total per dimension
  base_t **A;    // local submatrix
  base_t *x, *b;

  int nrows_a_loc, ncols_a_loc;
  int nrows_x_loc;
  int ncols_b_loc;

  // this offset defines the offset per rank
  // for mv1, it's the row offset
  // for mv2, it's the col offset
  int offset = 0;

  int i;
  int tok;
  int nset = 0;
  int nrep = 1;
  int check = 0;
  int help = 0;
  int pat = 1;
  double stime, rtime;

  opterr = 0;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  while ((tok = getopt(argc, argv, "n:r:p:vhc")) != -1) {
    switch (tok) {
    case 'n':
      n = atoi(optarg);
      nset = 1;
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
    case 'r':
      nrep = atoi(optarg);
      break;
    case 'p':
      pat = atoi(optarg);
      break;
    default:
      fprintf(stderr, "unknown parameter\n");
      exit(1);
    }
  }

  if (nset == 0 || help == 1) {
    if (rank == 0) {
      usage(basename(argv[0]));
      fflush(stdout);
    }
    return 1;
  }

  if (n < size) {
    if (rank == 0) {
      printf("n needs to be >= number of processes\n");
      usage(basename(argv[0]));
      fflush(stdout);
    }
  }

  create_instance(&A, n, n, &nrows_a_loc, &ncols_a_loc,
                  &x, n, &nrows_x_loc,
                  &b, n, &ncols_b_loc,
                  &offset,
                  pat);


  if (rank == 0) {
    printf("rep,n,p,t\n");
  }

  for (i = 0; i < nrep; i++) {

    // multiply
    clear_vector(b, ncols_b_loc);
    MPI_Barrier(MPI_COMM_WORLD);
    stime = MPI_Wtime();
    mv(A, n, n, nrows_a_loc, ncols_a_loc, x, nrows_x_loc, b, ncols_b_loc);
    rtime = MPI_Wtime() - stime;

    if (rank == 0) {
      MPI_Reduce(MPI_IN_PLACE, &rtime, 1, MPI_DOUBLE, MPI_MAX, 0,
                 MPI_COMM_WORLD);
      printf("%d,%d,%d,%f\n", i, n, size, rtime);
    } else {
      MPI_Reduce(&rtime, &rtime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    }

    fflush(stdout);
  }


  if (check == 1) {
    base_t **AA, *X, *B; // full matrices
    int i, j;
    int ret = 0;

    // generate full (r*n)x(r*m) input matrices (sequentially)
    alloc_2d_array(&AA, n, n);
    alloc_vector(&X, n);
    alloc_vector(&B, n);

    for(i=0; i<n; i++) {
      for(j=0; j<n; j++) {
        AA[i][j] = i*n+j;
      }
      X[i] = i;
    }

    for(i=0; i<n; i++) {
      for (j = 0; j < n; j++) {
        B[i] += AA[i][j] * X[j];
      }
    }

    // compare to local part
    ret = compare_vector_part(b, B, ncols_b_loc, offset);

    if( is_verbose()) {
      if( rank == 0 ) {
        printf("A: \n");
        print_2d_array(AA, n, stdout);

        printf("b (reference): \n");
        print_vector(B, n, stdout);
      }

      for(i=0; i<size; i++) {
        if( rank == i) {
          printf("%d: b (part): ", rank);
          print_vector(b, ncols_b_loc, stdout);
        }
        MPI_Barrier(MPI_COMM_WORLD);
      }
    }

    if (rank == 0) {
      MPI_Reduce(MPI_IN_PLACE, &ret, 1, MPI_INT, MPI_SUM, 0,
          MPI_COMM_WORLD);
      if( ret == 0 ) {
        printf("PASSED\n");
      } else {
        printf("NOT PASSED\n");
      }
    } else {
      MPI_Reduce(&ret, &ret, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    }

    free_2d_array(AA, n);
    free_vector(X);
    free_vector(B);

  }

  free_2d_array(A, nrows_a_loc);
  free_vector(x);
  free_vector(b);

  MPI_Finalize();
  return 0;
}
