#include <assert.h>
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "mv.h"

/*
(n*m/p) + m + p
*/
void mv(base_t **A, int nrows, int ncols, int nrows_a_loc, int ncols_a_loc, base_t *x, int nrows_x_loc, base_t *b, int ncols_b_loc) {
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    base_t *localB = (base_t *)malloc(ncols * sizeof(base_t));

    // n*m/p
    for (int i = 0; i < nrows; i++) {
        localB[i] = 0;
        for (int j = 0; j < ncols_a_loc; j++) {
            localB[i] += A[i][j] * x[j];
        }
    }

    int counts[size];
    // p + log(p)
    MPI_Allgather(&ncols_a_loc, 1, MPI_INT, counts, 1, MPI_INT, MPI_COMM_WORLD);

    // m + log(p)
    MPI_Reduce_scatter(localB, b, counts, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    free(localB);
}
