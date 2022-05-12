//
// Created by Sascha on 4/29/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "mv.h"
#include "mv_instance.h"

void create_instance(base_t ***A, int nrows_a, int ncols_a, int *nrows_a_loc, int *ncols_a_loc,
                    base_t **x, int nrows_x, int *nrows_x_loc,
                    base_t **b, int ncols_b, int *nrows_b_loc,
                    int *offset,  // offset in rows
                    int pattern)
{
  int rank, size;
  int i, j;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (pattern == 1) {
    *nrows_a_loc = nrows_a / size;
    *ncols_a_loc = ncols_a;

    if (rank < nrows_a % size) {
      (*nrows_a_loc)++;
    }

    // that should be equal
    *nrows_x_loc = *nrows_a_loc;
    *nrows_b_loc = *nrows_a_loc;

    MPI_Exscan(nrows_a_loc, offset, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    alloc_2d_array(A, *nrows_a_loc, *ncols_a_loc);
    alloc_vector(x, *nrows_x_loc);
    alloc_vector(b, *nrows_b_loc);

    for (i = 0; i < *nrows_a_loc; i++) {
      for (j = 0; j < ncols_a; j++) {
        (*A)[i][j] = (*offset + i) * ncols_a + j;
      }
      (*x)[i] = (*offset + i);
    }

//    if (is_verbose()) {
//      printf("%d: nloc=%d row_off=%d\n", rank, *nrows_a_loc, *offset);
//    }

  }

}
