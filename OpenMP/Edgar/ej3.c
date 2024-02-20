#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>

void prodmv(double *a, double *c, double **B, int size) {
    for (int i = 0; i < size; i++) {
        a[i] = 0.0;
        for (int j = 0; j < size; j++) {
            a[i] += B[i][j] * c[j];
        }
    }
}

double random_value(double min, double max){
    double r = (double)rand() / RAND_MAX;
    return min + r * (max - min);
}

int main(int argc, char *argv[]) {
    int i, j;
    double itime, ftime, exec_time;

    if (argc != 3) {
        fprintf(stderr, "Uso: %s <tamaño> <cantidad de hilos>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    int nthreads = atoi(argv[2]);

    omp_set_num_threads(nthreads);

    double *a = (double *)malloc(sizeof(double) * N);
    double *c = (double *)malloc(sizeof(double) * N);

    double **B = (double **)malloc(N * sizeof(double *));
    for (int i = 0; i < N; i++){
        B[i] = (double *)malloc(N * sizeof(double));
    }

    srand(time(NULL));
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            B[i][j] = random_value(1.0, 10.0);
    
    for (int i = 0; i < N; i++)
        c[i] = random_value(1.0, 10.0);

    itime = omp_get_wtime();

    #pragma omp parallel for private(i, j) shared(a, c, B, N)
        for (i = 0; i < N; i++) {
            a[i] = 0.0;
            for (j = 0; j < N; j++) {
                a[i] += B[i][j] * c[j];
            }
        }

    ftime = omp_get_wtime(); 
    exec_time = ftime - itime;
    printf("Tiempo de ejecución en paralelo: %.4f\n", exec_time);
    

    double time_spent = 0.0;
    clock_t begin = clock();

    double *a_serie = (double *)malloc(sizeof(double) * N);
    prodmv(a_serie, c, B, N);

    clock_t end = clock();
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;

    printf("Tiempo de ejecución en serie: %.4f\n", time_spent);
    
    return 0;
}
