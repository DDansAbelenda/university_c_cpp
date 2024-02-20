#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        printf("Son exactamente 2 parámetros: variable N y cantidad de hilos\n");
        return 1;
    }

    int N = atoi(argv[1]);
    int nthreads = atoi(argv[2]);

    omp_set_num_threads(nthreads);

    // Calculo de la cantidad de números primos de 1..N
    int i, j, prime, total = 0;
#pragma omp parallel for shared(N) private(i, j, prime) reduction(+ : total)
    for (i = 2; i <= N; i++)
    {
        prime = 1;
        for (j = 2; j < i; j++)
        {
            if (i % j == 0)
            {
                prime = 0;
                break;
            }
        }
        total += prime;
    }

    printf("La cantidad de números entre 1 y %d es: %d\n", N, total);

    return 0;
}