
#include "seq_common.h"

void seq_merge1(double a[], long n, double b[], long m, double c[]) {
  long i, j, k;

  i = 0;
  j = 0;
  k = 0;

  while (i < n && j < m) {
    c[k++] = (a[i] <= b[j]) ? a[i++] : b[j++];
  }

  while (i < n) {
    c[k++] = a[i++];
  }

  while (j < m) {
    c[k++] = b[j++];
  }

}

