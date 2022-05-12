/*
 * utils.c
 *
 *  Created on: Apr 14, 2020
 *      Author: sascha
 */

#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include "utils.h"

void alloc_2d_array(base_t ***m, int nrows, int ncols) {
  int i;
  *m = (base_t**) malloc(nrows * sizeof(base_t*));
  for(i=0; i<nrows; i++) {
    (*m)[i] = (base_t*) malloc(ncols * sizeof(base_t));
  }
}

void free_2d_array(base_t **m, int nrows) {
  for(int i=0; i<nrows; i++)
    free(m[i]);
  free(m);
}

void alloc_vector(base_t **x, int length) {
  *x = (base_t*) calloc(length, sizeof(base_t));
}

void free_vector(base_t *x) {
  free(x);
}

void clear_vector(base_t* x, int length) {
  int i;
  for (i = 0; i < length; i++) {
    x[i] = (base_t) 0;
  }
}




//int alloc_2d_matrix(base_t*** m, int n) {
//  int ret = 0;
//
//  base_t *array = NULL;
//
//  *m = (base_t**) malloc(n * sizeof(base_t*));
//  if (m == NULL) {
//    ret = -1;
//  } else {
//    int i;
//    array = (base_t*) malloc(n * n * sizeof(base_t));
//    if (array == NULL) {
//      free(*m);
//      ret = -1;
//    } else {
//      for (i = 0; i < n; i++) {
//        (*m)[i] = &array[i * n];
//      }
//    }
//  }
//
//  return ret;
//}
//
//int free_2d_matrix(base_t** m) {
//  int ret = 0;
//
//  if (m != NULL) {
//    free(m[0]);
//    free(m);
//  }
//
//  return ret;
//}
//
//void fill_dist_2d_matrix(base_t** m, int n, int roff, int coff, int total_dim) {
//  int i, j;
//  int rank;
//
//  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//  srand(rank);
//
//  for (i = 0; i < n; i++) {
//    for (j = 0; j < n; j++) {
//      //m[i][j] = j;
//      //m[i][j] = (i+roff) * total_dim + (j+coff);
//      //m[i][j] = i + j;
//      //m[i][j] = (base_t)1;
//      m[i][j] = ((double) rand() / (RAND_MAX));
//    }
//  }
//}
//
//void clear_2d_matrix(base_t** m, int n) {
//  int i, j;
//  for (i = 0; i < n; i++) {
//    for (j = 0; j < n; j++) {
//      m[i][j] = (base_t) 0;
//    }
//  }
//}
//

void print_2d_array(base_t** m, int n, FILE *out) {
  int i, j;
  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      fprintf(out, "%.3f  ", m[i][j]);
    }
    fprintf(out, "\n");
  }
  fprintf(out, "\n");
}

void print_vector(base_t* x, int n, FILE *out) {
  int i;
  for (i = 0; i < n; i++) {
    fprintf(out, "%.3f  ", x[i]);
  }
  fprintf(out, "\n");
}


//void mmm(base_t** a, base_t** b, base_t** c, int n) {
//  int i, j, k;
//
//  for (i = 0; i < n; i++) {
//    for (k = 0; k < n; k++) {
//      for (j = 0; j < n; j++) {
//        c[i][j] += a[i][k] * b[k][j];
//      }
//    }
//  }
//
//}

void copy_2d_submatrix(base_t **a, base_t **A, int row, int n, int col, int m) {
  int i, j;

  for (i = 0; i < n; i++) {
    for (j = 0; j < m; j++) {
      a[i][j] = A[row + i][col + j];
    }
  }
}

int compare_vector_part(base_t *vec_part, base_t *full_vec, int part_length, int offset) {
  int i;
  int difference = 0;
  int rank;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  for (i = 0; i < part_length; i++) {
    if ( fabs(vec_part[i] - full_vec[offset+i]) > 1e-9 ) {
      printf("%d: b[%d]=%g != B[%d]=%g\n", rank, i, vec_part[i], offset + i, full_vec[offset + i]);
      difference = 1;
      break;
    }
  }
  return difference;
}
