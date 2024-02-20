#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int main(int argc, char *argv[])
{
    int size, nthreads;

    if (argc != 3)
    {
        printf("Son exactamente 2 parámetros: tamaño de la matriz y cantidad de hilos\n");
        return 1;
    }

    size = atoi(argv[1]);
    nthreads = atoi(argv[2]);
    omp_set_num_threads(nthreads);

    // Inicialización de la matriz
    int **matrix = (int **)malloc(sizeof(int *) * size);
    for (int i = 0; i < size; i++)
    {
        matrix[i] = (int *)malloc(sizeof(int) * size);
    }

    // Llenado de la matriz
    srand(time(NULL));
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            matrix[i][j] = rand() % 10;
        }
    }

    // Trabajando paralelo
    int min_value = matrix[0][0];
    int max_value = matrix[0][0];

#pragma omp parallel for shared(size, matrix, max_value, min_value)
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
#pragma omp critical
            {

                if (matrix[i][j] < min_value)
                {
                    min_value = matrix[i][j];
                }
                if (matrix[i][j] > max_value)
                {
                    max_value = matrix[i][j];
                }
            }
        }
    }

    printf("El mayor elemento es: %d, y el menor: %d \n", max_value, min_value);

    return 0;
}
