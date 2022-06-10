#include <mpi.h>

void mv(base_t **A, int nrows, int ncols, int nrows_a_loc, int ncols_a_loc,
        base_t *x, int ncols_x_loc,
        base_t *b, int nrows_b_loc){

            int rank, size;
            MPI_Comm_size(MPI_COMM_WORLD,&size);
            MPI_Comm_rank(MPI_COMM_WORLD,&rank);
            
            int chonk = nrows/size;
            if (rank == size-1){
                chonk += nrows%size;
            }

            MPI_DOUBLE* sendbuf = malloc(ncols * sizeof(MPI_DOUBLE)) //sendbuf is "result"

            int[size] recvcounts;
            for(int i = 0; i < size; i++){
                recvcounts[i] = chonk;
            }// we have size processes - recvcounts[0] indicates the number of data process 0 receives
            
            int[size] displs; // we have size processes - recvcounts[0] indicates the start from where process 0 receives data from sendbuf
            for(int i = 0; i < size; i++){
                displs[i] = i * chonk;
            }

            //error handling?
            MPI_Allgatherv(sendbuf, chonk, MPI_DOUBLE,
            b, recvcounts, displs, MPI_DOUBLE, MPI_COMM_WORLD);

            for(int i = 0; i < chonk; i++){
                b[i] = 0;
                for(int j = 0; j < ncols; i++){
                    b[i] += A[i][j]*x[j];
                }
            }
            free(sendbuf);
        }

int main(int argc, char[][] argv)
{

    //TODO: proper argument parsing
    MPI_DOUBLE **A = argv[1];
    MPI_DOUBLE *x = argv[2];
    MPI_DOUBLE *b = malloc(ncols * sizeof(MPI_DOUBLE));

    //error handling?
    MPI_Init(&argc, &argv);
    mv(A,nrows,ncols,nrows_a_loc,ncols_a_loc,x,ncols_x_loc,b,nrows_b_loc);

    MPI_Finalize();
    fprint(&b);
    free(b);
}