#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv)
{
    int size, rank, rows = 1048576, columns = 10, matrix_size, rows_lpm, lpm_size;
    double start_time, end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    start_time = MPI_Wtime();

    int *global_matrix = NULL;      // Matriz global
    int *sum_total_matrix = NULL;   // Vector que almacena todas las sumas por fila
    int *local_procc_matrix = NULL; // Matriz de cada proceso
    int *lpm_sum = NULL;            // vector que almacena las sumas por proceso

    if (rank == 0)
    {
        // Tomando filas y columnas de la matriz
        /*printf("Indique la cantidad de filas, debe ser múltiplo de %d: ", size);
        scanf("%d", &rows);
        printf("Indique la cantidad de columnas: ");
        scanf("%d", &columns);*/

        // Tamaño de la matriz
        matrix_size = rows * columns;

        // Verificar si la cantidad de filas es correcta
        if (rows % size != 0)
        {
            printf("La cantidad de filas debe ser múltiplo de %d", size);
            fflush(stdout);
            MPI_Finalize();
            return 0;
        }

        // Cantidad de filas por proceso
        rows_lpm = rows / size;

        // Tamaño de la matriz por proceso
        lpm_size = rows_lpm * columns;

        // Asignar memoria para la matriz en su representación de vector
        global_matrix = (int *)malloc(matrix_size * sizeof(int));

        // Semillero
        srand(time(NULL));

        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < columns; j++)
            {
                global_matrix[i * columns + j] = rand() % 10;
            }
        }

        // Mostrar datos de la matriz global
        /*printf("La matriz generada es: \n");
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < columns; j++)
            {
                printf("%d ", global_matrix[i * columns + j]);
            }
            printf("\n");
        }
        printf("\n");*/
    }

    /*Envio de datos a los procesos para las sumas parciales*/

    // Enviando a todos los procesos la cantidad de columnas, las filas por proceso y el tamaño de las matrices
    // por cada proceso
    MPI_Bcast(&lpm_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&rows_lpm, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&columns, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Dar tamaño a la matriz local de cada proceso
    local_procc_matrix = (int *)malloc(lpm_size * sizeof(int));

    // Dar tamaño a la matriz de suma local de cada proceso
    lpm_sum = (int *)malloc(rows_lpm * sizeof(int));

    // Distribuir una cantidad igual de elementos(lpm_size) por procesos
    MPI_Scatter(global_matrix, lpm_size, MPI_INT, local_procc_matrix, lpm_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Sumar las filas de la matriz
    for (int i = 0; i < rows_lpm; i++)
    {
        lpm_sum[i] = 0;
        for (int j = 0; j < columns; j++)
        {
            lpm_sum[i] += local_procc_matrix[i * columns + j];
        }
    }

    /*Recepción de las sumas parciales*/
    sum_total_matrix = (int *)malloc(rows * sizeof(int));

    MPI_Gather(lpm_sum, rows_lpm, MPI_INT, sum_total_matrix, rows_lpm, MPI_INT, 0, MPI_COMM_WORLD);

    // Liberar memoria para los procesos
    free(local_procc_matrix);
    free(lpm_sum);

    // Mostrar la matriz de las sumas global
    if (rank == 0)
    {
        /*printf("El vector de sumas de la matriz es: ");
        for (int i = 0; i < rows; i++)
        {
            printf("%d ", sum_total_matrix[i]);
        }
        printf("\n");
        */
        // Liberar memoria de las matrices globales
        free(sum_total_matrix);
        free(global_matrix);
    }
    end_time = MPI_Wtime();
    printf("Tiempo de ejecucion %f", end_time - start_time);

    MPI_Finalize();
    return 0;
}