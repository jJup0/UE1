#include <assert.h>
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "mv.h"

/*
 A: Pointer to the input matrix (only the Submatrix the process has access to)
 nrows: number of rows of A
 ncols: number of columns of A
 nrows_a_loc: how many rows the current process received
 ncols_a_loc: how many columns (per row) the current process received (=ncols)
 x: Pointer to the columns of the input vector (where the process starts)
 ncols_x_loc: how many entries of the input vector the process receives
 b: pointer to the parts of the result vector the process writes on
 nrows_b_loc: how many entries of b the process writes on
*/

/*
(n*m/p) + n + p
*/
void mv(base_t **A, int nrows, int ncols, int nrows_a_loc, int ncols_a_loc, base_t *x, int nrows_x_loc, base_t *b, int ncols_b_loc) {
    int rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    base_t *completeX = (base_t *)malloc(nrows * sizeof(base_t));
    int *recvcounts = (int *)malloc(size * sizeof(int));
    int *displs = (int *)malloc(size * sizeof(int));

    displs[0] = 0;
    // p + log(p)
    MPI_Exscan(&nrows_x_loc, &displs[rank], 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    // p + log(p)
    MPI_Allgather(&nrows_x_loc, 1, MPI_INT, recvcounts, 1, MPI_INT, MPI_COMM_WORLD);

    // p + log(p)
    MPI_Allgather(&displs[rank], 1, MPI_INT, displs, 1, MPI_INT, MPI_COMM_WORLD);

    // n + log(p)
    MPI_Allgatherv(x, nrows_x_loc, MPI_DOUBLE, completeX, recvcounts, displs, MPI_DOUBLE, MPI_COMM_WORLD);

    // m*n/p
    for (int i = 0; i < ncols_b_loc; i++) {
        b[i] = 0;
        for (int j = 0; j < ncols; j++) {
            b[i] += A[i][j] * completeX[j];
        }
    }
    free(completeX);
    free(recvcounts);
    free(displs);
}
