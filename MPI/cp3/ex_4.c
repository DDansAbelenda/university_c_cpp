#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv)
{
    int size, rank, rows, columns, matrix_size;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int *global_matrix = NULL;      // Matriz global
    int *sum_total_matrix = NULL;   // Vector que almacena todas las sumas por fila
    int *local_procc_matrix = NULL; // Matriz de cada proceso
    int *lpm_sum = NULL;            // vector que almacena las sumas por proceso

    if (rank == 0)
    {
        // Tomando filas y columnas de la matriz
        printf("Indique la cantidad de filas: ");
        scanf("%d", &rows);
        printf("Indique la cantidad de columnas, debe ser múltiplo de %d: ", size);
        scanf("%d", &columns);

        // Tamaño de la matriz
        matrix_size = rows * columns;

        // Verificar si la cantidad de columnas es correcta
        if (columns % size != 0)
        {
            printf("La cantidad de columnas debe ser múltiplo de %d", size);
            fflush(stdout);
            MPI_Finalize();
            return 0;
        }

        // Asignar memoria para la matriz en su representación de vector
        global_matrix = (int *)malloc(matrix_size * sizeof(int));

        // Llenar la matriz

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
        printf("La matriz es: \n");
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < columns; j++)
            {
                printf("%d ", global_matrix[i * columns + j]);
            }
            printf("\n");
        }
        printf("\n");
    }

    /*Envio de datos a los procesos para las sumas parciales*/

    // Enviando a todos los procesos la cantidad de filas y columnas
    MPI_Bcast(&columns, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calcular cantidad de elementos por procesos y columnas por procesos
    int element_by_process = rows * columns / size;
    int columns_by_process = columns / size;

    // Dar tamaño a la matriz local de cada proceso
    local_procc_matrix = (int *)malloc(element_by_process * sizeof(int));

    // Definir tipo de dato variado vector
    MPI_Datatype col, column_t;
    MPI_Type_vector(rows, 1, columns, MPI_INT, &col);
    MPI_Type_commit(&col);
    MPI_Type_create_resized(col, 0, sizeof(int), &column_t);
    MPI_Type_commit(&column_t);

    // Distribuir una cantidad igual de elementos(lpm_size) por procesos
    MPI_Scatter(global_matrix, columns / size, column_t, local_procc_matrix, rows * columns / size, MPI_INT, 0, MPI_COMM_WORLD);

    // Dar tamaño a la matriz de suma local de cada proceso
    lpm_sum = (int *)malloc(columns_by_process * sizeof(int));

    // Sumar las columnas de la matriz
    for (int i = 0; i < columns_by_process; i++)
    {
        lpm_sum[i] = 0;
        for (int j = 0; j < rows; j++)
        {
            lpm_sum[i] += local_procc_matrix[i * rows + j];
        }
    }

    /*Recepción de las sumas parciales*/
    if (rank == 0)
    {
        sum_total_matrix = (int *)malloc(columns * sizeof(int));
    }
    MPI_Gather(lpm_sum, columns_by_process, MPI_INT, sum_total_matrix, columns_by_process, MPI_INT, 0, MPI_COMM_WORLD);

    // Mostrar la matriz de las sumas global
    if (rank == 0)
    {
        printf("El vector de sumas de la matriz es: ");
        for (int i = 0; i < columns; i++)
        {
            printf("%d ", sum_total_matrix[i]);
        }
        printf("\n");
        // Liberar memoria de las matrices globales
        free(sum_total_matrix);
        free(global_matrix);
    }
    // Liberar memoria para los procesos
    free(local_procc_matrix);
    free(lpm_sum);

    MPI_Type_free(&col);
    MPI_Type_free(&column_t);

    MPI_Finalize();
    return 0;
}