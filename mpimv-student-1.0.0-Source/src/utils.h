/*
 * utils.h
 *
 *  Created on: Apr 14, 2020
 *      Author: sascha
 */

#ifndef U2_MMM_SRC_UTILS_H_
#define U2_MMM_SRC_UTILS_H_

#include <stdio.h>
#include <mpi.h>

#include "mv.h"

void alloc_2d_array(base_t ***m, int nrows, int ncols);
void free_2d_array(base_t **m, int nrows);
void print_2d_array(base_t** m, int n, FILE *out);

void alloc_vector(base_t **x, int length);
void free_vector(base_t *x);
void print_vector(base_t* m, int n, FILE *out);
void clear_vector(base_t* x, int length);

int compare_vector_part(base_t *vec_part, base_t *full_vec, int part_length, int offset);

#endif /* U2_MMM_SRC_UTILS_H_ */
