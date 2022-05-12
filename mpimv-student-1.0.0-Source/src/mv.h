
#ifndef MV_H_
#define MV_H_

//typedef int base_t;
typedef double base_t;
extern MPI_Datatype mpi_datatype;

int is_verbose();

void mv(base_t **A, int nrows, int ncols, int nrows_a_loc, int ncols_a_loc,
        base_t *x, int nrows_x_loc,
        base_t *b, int ncols_b_loc);

#endif
