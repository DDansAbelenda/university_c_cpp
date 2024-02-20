#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char *argv[]) {
    int i, j, prime, total = 0;

    if (argc != 3) {
        fprintf(stderr, "Uso: %s <rango> <cantidad de hilos>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    int nthreads = atoi(argv[2]);

    omp_set_num_threads(nthreads);

    #pragma omp parallel for private(i, j, prime) shared(N) reduction(+:total)
    for (i = 2; i <= N; i++) {
        prime = 1;
        for (j = 2; j < i; j++) {
            if (i % j == 0) {
                prime = 0;
                break;
            }
        }
        total += prime;
    }

    printf("Cantidad de números primos: %d\n", total);

    return 0;
}
