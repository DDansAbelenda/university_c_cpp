/******************************************************************************
 * FILE: omp_error_1.c
 ******************************************************************************/
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#define N 50
#define CHUNKSIZE 5

int main(int argc, char *argv[])
{
  int i, chunk, tid;
  float a[N], b[N], c[N];

  /* Some initializations */
  for (i = 0; i < N; i++)
    a[i] = b[i] = i * 1.0;
  chunk = CHUNKSIZE;
  tid = omp_get_thread_num(); // se saca de dentro del for
// Se pone el ciclo for inmediatamente después de la directiva for de OpenMP
#pragma omp parallel for shared(a, b, c, chunk) private(i, tid) schedule(static, chunk)
  for (i = 0; i < N; i++)
    {
      c[i] = a[i] + b[i];
      printf("tid= %d i= %d c[i]= %f\n", tid, i, c[i]);
    }
  /* end of parallel for construct */
  return 0;
}
