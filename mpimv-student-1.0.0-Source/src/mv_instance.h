//
// Created by Sascha on 4/29/22.
//

#ifndef E1_CODE_MVMPI_SRC_MV1_INSTANCE_H
#define E1_CODE_MVMPI_SRC_MV1_INSTANCE_H

#include "mv.h"
#include "utils.h"

// all variables _loc are output variables and will be set
// also all pointes to arrays will be initialized

void create_instance(base_t ***A, int nrows_a, int ncols_a, int *nrows_a_loc, int *ncols_a_loc,
                     base_t **x, int nrows_x, int *nrows_x_loc,
                     base_t **b, int ncols_b, int *nrows_b_loc,
                     int *offset,  // output variable
                     int pattern);


#endif //E1_CODE_MVMPI_SRC_MV1_INSTANCE_H
