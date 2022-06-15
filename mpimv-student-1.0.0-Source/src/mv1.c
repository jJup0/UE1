#include <assert.h>
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "mv.h"
#include "utils.h"

void printIntArray(int *array, int n, int rank) {
    fprintf(stderr, "Rank = %d  ", rank);
    fprintf(stderr, "[%d", array[0]);
    for (int i = 1; i < n; i++) {
        fprintf(stderr, ", %d", array[i]);
    }
    fprintf(stderr, "]\n");
}

void printDoubleArray(double *array, int n, int rank) {
    fprintf(stderr, "Rank = %d  ", rank);
    fprintf(stderr, "[%.0f", array[0]);
    for (int i = 1; i < n; i++) {
        fprintf(stderr, ", %.0f", array[i]);
    }
    fprintf(stderr, "]\n");
}

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
(n*m/p) + m + p
*/
void mv(base_t **A, int nrows, int ncols, int nrows_a_loc, int ncols_a_loc, base_t *x, int nrows_x_loc, base_t *b, int ncols_b_loc) {
    int rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    base_t *completeX;
    base_t *recvcounts;
    base_t *displs;
    alloc_vector(&completeX, nrows);
    alloc_vector(&recvcounts, size);
    alloc_vector(&displs, size);
    displs[0] = 0;
    // p + log(p)
    MPI_Exscan(&nrows_x_loc, &displs[rank], 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    // p + log(p)
    MPI_Allgather(&nrows_x_loc, 1, MPI_INT, recvcounts, 1, MPI_INT, MPI_COMM_WORLD);

    // p + log(p)
    MPI_Allgather(&displs[rank], 1, MPI_INT, displs, 1, MPI_INT, MPI_COMM_WORLD);

    // m + log(p)
    MPI_Allgatherv(x, nrows_x_loc, MPI_DOUBLE, completeX, recvcounts, displs, MPI_DOUBLE, MPI_COMM_WORLD);

    // m*n/p
    for (int i = 0; i < ncols_b_loc; i++) {
        b[i] = 0;
        for (int j = 0; j < ncols; j++) {
            b[i] += A[i][j] * completeX[j];
        }
    }
    free_vector(completeX);
    free_vector(recvcounts);
    free_vector(displs);
}
