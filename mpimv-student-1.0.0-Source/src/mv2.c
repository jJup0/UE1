#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <math.h>

#include <mpi.h>

#include "utils.h"
#include "mv.h"

void mv(base_t **A, int nrows, int ncols, int nrows_a_loc, int ncols_a_loc,
        base_t *x, int nrows_x_loc,
        base_t *b, int ncols_b_loc)
{

    int rank, size;
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    base_t *localB;
    alloc_vector(&localB, ncols);


    for (int i = 0; i < nrows; i++)
    {
        localB[i] = 0;
        for (int j = 0; j < ncols_a_loc; j++)
        {
          localB[i] += A[i][j] * x[j];
        }
    }

    int counts[size];
    MPI_Allgather(&ncols_a_loc, 1, MPI_INT, counts, 1, MPI_INT, MPI_COMM_WORLD);

    MPI_Reduce_scatter(localB, b, counts, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
}
