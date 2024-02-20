#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

void randInit(int vector[], const int n, const int bound);

double mean(int vector[], const int n);

int main(int argc, char *argv[])
{
    int size, nthreads;

    if (argc != 3)
    {
        printf("Son exactamente 2 parámetros: tamaño del arreglo y cantidad de hilos\n");
        return 1;
    }

    size = atoi(argv[1]);
    nthreads = atoi(argv[2]);

    printf("Tamaño del vector: %d\nCantidad de hilos:%d\n\n", size, nthreads);
    omp_set_num_threads(nthreads);

    int *vector = (int *)malloc(sizeof(int) * size);

    randInit(vector, size, 100);

    // Trabajando paralelo
    double start, end, time;
    start = omp_get_wtime(); // inicio

    double p_sum = 0.0;
    double p_mean = 0.0;

#pragma omp parallel for reduction(+ : p_sum)
    for (int i = 0; i < size; i++)
    {
        p_sum += vector[i];
    }

    end = omp_get_wtime(); // fin
    time = end - start;

    printf("Paralelo\n");
    printf("Tiempo de ejecución: %f\n", time);
    p_mean = p_sum / size;
    
    printf("Suma: %f\nPromedio: %f\n", p_sum, p_mean);
    printf("------------------------\n");

    // Trabajando en serie
    
    double time_spent = 0.0;
    clock_t begin = clock();

    double l_mean = mean(vector, size);

    clock_t s_end = clock();
    time_spent += (double)(s_end - begin) / CLOCKS_PER_SEC;
   
    printf("Serie\n");
    printf("Tiempo de ejecución: %f\n", time_spent);
    printf("Promedio: %f\n", l_mean);
    
    //Speedup
    printf("------------------------\n");  
    printf("Speedup: %.4f\n", (time_spent / time));
    
    

    // Liberar memoria
    free(vector);

    return 0;
}

void randInit(int array[], const int n, const int bound)
{
    srand(time(NULL));
    for (int i = 0; i < n; i++)
        array[i] = rand() % bound;
}
double mean(int array[], const int n)
{
    double sum = 0;
    for (int i = 0; i < n; i++)
        sum += array[i];
    return (double)sum / n;
}