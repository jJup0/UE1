/*
 * parmerge.c
 *
 *  Created on: Mar 12, 2019
 *      Author: sascha
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include "merge.h"

void merge(double a[], long n, double b[], long m, double c[]) {

  // replace this by a parallel merge algorithm
  seq_merge1(a, n, b, m, c);

}


