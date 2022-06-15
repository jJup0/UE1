/**
 * @file mv1.c
 * @author Elias Pinter <12023962>, Kurdo-Jaroslav Asinger <01300351>, Jakob Roithinger <52009269>
 * @brief Program mv1: This program multiplies the matrix A with the column-vector x and stores the result into b.
 * MPI_INIT must have taken place before the call of mv, and each process has to call it.
 * @details The partition, which process takes care of which part of the matrix has to be done in before.
 * The information about that is passed in the arguments.
 **/

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

/**
 * @brief This function performs a matrix-vector multiplication.
 * @param A pointer to the sub-matrix of a matrix to be multiplies with a vector
 * @param nrows the number of rows of the original matrix (not A) 
 * @param ncols the number of columns of the original matrix (not A) 
 * @param nrows_a_loc the number of rows this processor has to process the multiplication with
 * @param ncols_a_loc the number of columns this processor has to process the multiplication with (unnecessary here)
 * @param x pointer to a part of the original vector which is multiplied to the original matrix
 * @param nrows_x_loc the number of rows of the original vector whiswere designated to the processor
 * @param b pointer to the result vector b
 * @param ncols_b_loc the number of rows of b which was designated to this processor (unnecessary here)
 */
void mv(base_t **A, int nrows, int ncols, int nrows_a_loc, int ncols_a_loc, base_t *x, int nrows_x_loc, base_t *b, int ncols_b_loc) {
    int rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    base_t *completeX = (base_t *)malloc(nrows * sizeof(base_t));
    int *recvcounts = (int *)malloc(size * sizeof(int));
    int *displs = (int *)malloc(size * sizeof(int));

    displs[0] = 0;
    MPI_Exscan(&nrows_x_loc, &displs[rank], 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    MPI_Allgather(&nrows_x_loc, 1, MPI_INT, recvcounts, 1, MPI_INT, MPI_COMM_WORLD);

    MPI_Allgather(&displs[rank], 1, MPI_INT, displs, 1, MPI_INT, MPI_COMM_WORLD);

    MPI_Allgatherv(x, nrows_x_loc, MPI_DOUBLE, completeX, recvcounts, displs, MPI_DOUBLE, MPI_COMM_WORLD);

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
