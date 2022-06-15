/**
 * @file mv2.c
 * @author Elias Pinter <12023962>, Kurdo-Jaroslav Asinger <01300351>, Jakob Roithinger <52009269>
 * @brief Program mv2: This program multiplies the matrix A with the column-vector x and stores the result into b.
 * MPI_INIT must have taken place before the call of mv, and each process has to call it.
 * @details The partition, which process takes care of which part of the matrix has to be done in before.
 * The information about that is passed in the arguments.
 * Each process receives a part of complete columns of the original matrix and the complete original vector.
 **/

#include <assert.h>
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "mv.h"

/**
 * @brief This function performs a matrix-vector multiplication.
 * @param A pointer to the sub-matrix of a matrix to be multiplies with a vector
 * @param nrows the number of rows of the original matrix (not A)
 * @param ncols the number of columns of the original matrix (not A)
 * @param nrows_a_loc the number of rows this processor has to process the multiplication with (unnecessary here)
 * @param ncols_a_loc the number of columns this processor has to process the multiplication with
 * @param x pointer to a part of the original vector which is multiplied to the original matrix
 * @param nrows_x_loc the number of rows of the original vector whiswere designated to the processor (unnecessary here)
 * @param b pointer to a part of the result vector b
 * @param ncols_b_loc the number of rows of b which was designated to this processor (length of b, equal to nrows_a_loc)
 */
void mv(base_t **A, int nrows, int ncols, int nrows_a_loc, int ncols_a_loc, base_t *x, int nrows_x_loc, base_t *b, int ncols_b_loc) {
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    base_t *localB = (base_t *)malloc(ncols * sizeof(base_t));

    for (int i = 0; i < nrows; i++) {
        localB[i] = 0;
        for (int j = 0; j < ncols_a_loc; j++) {
            localB[i] += A[i][j] * x[j];
        }
    }

    int counts[size];
    MPI_Allgather(&ncols_a_loc, 1, MPI_INT, counts, 1, MPI_INT, MPI_COMM_WORLD);

    MPI_Reduce_scatter(localB, b, counts, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    free(localB);
}
