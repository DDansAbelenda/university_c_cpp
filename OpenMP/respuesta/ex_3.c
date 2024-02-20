
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

void prodmv(double *a, double *c, double **B, int size);

int main(int argc, char *argv[])
{
    int N, nthreads;

    if (argc != 3)
    {
        printf("Son exactamente 2 parámetros: dimensión N y cantidad de hilos\n");
        return 1;
    }

    N = atoi(argv[1]);
    nthreads = atoi(argv[2]);
    printf("Dimensión de la matriz: %d\nCantidad de hilos:%d\n\n", N, nthreads);

    omp_set_num_threads(nthreads);

    // Pidiendo memoria para los vectores a y c, y la matriz B
    double *a = (double *)malloc(sizeof(double) * N);
    double *c = (double *)malloc(sizeof(double) * N);
    double **B = (double **)malloc(sizeof(double *) * N);
    for (int i = 0; i < N; i++)
    {
        B[i] = (double *)malloc(sizeof(double) * N);
    }

    // Rellenando los arreglos con valores random
    srand(time(NULL));
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            B[i][j] = rand() % 10;

    for (int i = 0; i < N; i++)
        c[i] = rand() % 10;

    // Trabajando paralelo
    double start, end, time;
    start = omp_get_wtime(); // inicio

    double p_sum = 0.0;
    double p_mean = 0.0;

#pragma omp parallel for shared(a, c, B, N)
    for (int i = 0; i < N; i++)
    {
        a[i] = 0;
        for (int j = 0; j < N; j++)
        {
            a[i] += B[i][j] * c[j];
        }
    }

    end = omp_get_wtime(); // fin
    time = end - start;

    printf("Paralelo\n");
    printf("Tiempo de ejecución: %f\n", time);
    printf("----------------\n");
   
    // Trabajando en serie

    double time_spent = 0.0;
    clock_t begin = clock();

    double *a_2 = (double *)malloc(sizeof(double) * N);
    prodmv(a_2, c, B, N);

    clock_t s_end = clock();
    time_spent += (double)(s_end - begin) / CLOCKS_PER_SEC;

    printf("Secuencial\n");
    printf("Tiempo de ejecución: %f\n", time_spent);
    
    //Speedup
    printf("------------------------\n");  
    printf("Speedup: %.4f\n", (time_spent / time));
    

    // Liberar memoria
    free(B);
    free(c);
    free(a);
    free(a_2);

    return 0;
}

void prodmv(double *a, double *c, double **B, int size)
{
    for (int i = 0; i < size; i++)
    {
        a[i] = 0.0;
        for (int j = 0; j < size; j++)
        {
            a[i] += B[i][j] * c[j];
        }
    }
}
