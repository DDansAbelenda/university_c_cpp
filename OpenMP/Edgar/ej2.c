#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>

void randInit(int array[], const int n, const int bound) {
    srand(time(NULL));
    for (int i = 0; i < n; i++)
        array[i] = rand() % bound;
}

double mean(int array[], const int n) {
    double sum = 0;
    for (int i = 0; i < n; i++)
        sum += array[i];
    return (double)sum / n;
}

int main(int argc, char *argv[]) {
    int i;
    double parallel_sum = 0.0;
    double parallel_mean = 0.0;

    if (argc != 3) {
        fprintf(stderr, "Uso: %s <tamaño del arreglo> <cantidad de hilos>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int nthreads = atoi(argv[2]);
    omp_set_num_threads(nthreads);

    int *array = (int *)malloc(sizeof(int) * n);

    randInit(array, n, 100);

    double itime, ftime, exec_time; 
    itime = omp_get_wtime();

    #pragma omp parallel for private(i) shared(array, n) reduction(+:parallel_sum)
    for (i = 0; i < n; i++) {
        parallel_sum += array[i];
    }

    ftime = omp_get_wtime(); 
    exec_time = ftime - itime; 
    printf("Tiempo de ejecución en paralelo: %.4f\n", exec_time);

    parallel_mean = parallel_sum / n;
    printf("Media calculada en paralelo: %.2f\n", parallel_mean);


    double time_spent = 0.0;
    clock_t begin = clock();

    double serial_mean = mean(array, n);

    clock_t end = clock();
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Tiempo de ejecución en serie: %.4f\n", time_spent);

    printf("Media calculada en serie: %.2f\n", serial_mean);

    return 0;
}
