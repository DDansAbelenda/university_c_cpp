#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int main(int argc, char *argv[]) {
    int i, j, min, max;

    if (argc != 3) {
        fprintf(stderr, "Uso: %s <tamaño de la matriz> <cantidad de hilos>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    int nthreads = atoi(argv[2]);

    omp_set_num_threads(nthreads);

    int **matrix = (int **)malloc(N * sizeof(int *));
    for (int i = 0; i < N; i++){
        matrix[i] = (int *)malloc(N * sizeof(int));
    }

    srand(time(NULL));
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            matrix[i][j] = rand() % 100;

    min = matrix[0][0];
    max = matrix[0][0];

    #pragma omp parallel for shared(N, matrix, min, max) private(i, j)
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            #pragma omp critical
            {
                if (matrix[i][j] < min) {
                    min = matrix[i][j];
                }
                if (matrix[i][j] > max) {
                    max = matrix[i][j];
                }
            }
        }
    }

    printf("El menor elemento de la matriz es: %d\n", min);
    printf("El mayor elemento de la matriz es: %d\n", max);

    return 0;
}
