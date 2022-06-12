

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <math.h>

#include <mpi.h>

#include "utils.h"
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
void mv(base_t **A, int nrows, int ncols, int nrows_a_loc, int ncols_a_loc,
        base_t *x, int nrows_x_loc,
        base_t *b, int ncols_b_loc)
{

    int rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_DOUBLE[ncols_x_loc] sendbuf = x;

    /*
    int chonk = nrows/size;
    if (rank == size-1){
        chonk += nrows % size;
    }

    int[size] recvcounts;
    for(int i = 0; i < size; i++){
        recvcounts[i] = chonk;
    }

    int[size] displs;
    for(int i = 0; i < size; i++){
        displs[i] = i * chonk;
    }
    */

    int[size] displs;
    displs[rank] = ncols_x_loc;
    MPI_Allgather(displs, 1, int, displs, 1, int, MPI_COMM_WORLD);

    int[site] recvcounts;
    if (rank == size - 1)
    {
        recvcounts[rank] = ncols - displs[rank];
    }
    else
    {
        recvcounts[rank] = displs[rank + 1] - displs[rank];
    }
    MPI_Allgather(recvcounts, 1, int, recvcounts, 1, int, MPI_COMM_WORLD);

    // error handling?
    MPI_Allgatherv(sendbuf, recvcounts[rank], MPI_DOUBLE,
                   x, recvcounts, displs, MPI_DOUBLE, MPI_COMM_WORLD);

    for (int i = ncols_x_loc; i < recvcounts[rank]; i++)
    {
        b[i] = 0;
        for (int j = 0; j < ncols; i++)
        {
            b[i] += A[i][j] * x[j];
        }
    }
    free(sendbuf);
}
