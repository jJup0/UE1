

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

            // allocate scatter buffer only on root
            if(rank==0){

            }
            //get sendcounts with allgather
            //get displs with allgather
            //recvbuf much smaller - what?
            //we have own recvcount
            // scatterv
            MPI_Scatterv(A, const int *sendcounts,
            const int *displs, MPI_Datatype sendtype,
            void *recvbuf, int recvcount, MPI_Datatype recvtype,
            int root, MPI_Comm comm);


            MPI_DOUBLE[ncols_x_loc] sendbuf = x;


            int[size] displs;
            displs[rank] = ncols_x_loc;
            MPI_Allgather(displs, 1, int, displs, 1, int, MPI_COMM_WORLD);

            int[site] recvcounts;
            if(rank == size - 1){
                recvcounts[rank] = ncols - displs[rank];
            }
            else{
                recvcounts[rank] = displs[rank + 1] - displs[rank];
            }
            MPI_Allgather(recvcounts, 1, int, recvcounts, 1, int, MPI_COMM_WORLD);
            

            //error handling?
            MPI_Allgatherv(sendbuf, recvcounts[rank], MPI_DOUBLE,
            x, recvcounts, displs, MPI_DOUBLE, MPI_COMM_WORLD);

            for(int i = ncols_x_loc; i < recvcounts[rank]; i++){
                b[i] = 0;
                for(int j = 0; j < ncols; i++){
                    b[i] += A[i][j]*x[j];
                }
            }
            free(sendbuf);
        }
