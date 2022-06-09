#include <mpi.h>

void mv(base_t **A, int nrows, int ncols, int nrows_a_loc, int ncols_a_loc,
        base_t *x, int ncols_x_loc,
        base_t *b, int nrows_b_loc){

            int rank, size;
            MPI_Comm_size(MPI_COMM_WORLD,&size);
            MPI_Comm_rank(MPI_COMM_WORLD,&rank);

            int chonk = nrows/MPI_Comm_size;

            MPI_DOUBLE[nrows*ncols + ncols_x_loc] sendbuf; //we send all numbers of the matrix and the vector
            //TODO: fill values from A into sendbuf

            int[size] recvcounts;
            for(int i = 0; i < size-1; i++){
                recvcounts[i] = chonk;
            }

            recvcounts[size-1] = chonk + nrows%size; //we have size processes - recvcounts[0] indicates the number of data process 0 receives
            int[size] displs; //we have size processes - recvcounts[0] indicates the start from where process 0 receives data from sendbuf
            for(int i = 0; i < size; i++){
                displs[i] = i * chonk;
            }

            //error handling?
            MPI_Allgatherv(sendbuf, nrows*ncols + ncols_x_loc, MPI_DOUBLE,
            b, recvcounts, displs, MPI_DOUBLE, MPI_COMM_WORLD);

            //compute
        }

int main(int argc, char[][] argv)
{
    //error handling?
    MPI_Init(&argc, &argv);

    //TODO: allocate pointers and call mv() here
    MPI_Finalize();
}