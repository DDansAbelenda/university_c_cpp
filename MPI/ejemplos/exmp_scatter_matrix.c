#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int size, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int *global_matrix = NULL;
    int *local_procc_matrix = NULL;
    int rows, columns, matrix_size, rows_lpm, lpm_size;
    if (rank == 0)
    {
        // Tomando filas y columnas de la matriz
        printf("Indique la cantidad de filas, debe ser múltiplo de %d: ", size);
        scanf("%d", &rows);
        printf("Indique la cantidad de columnas: ");
        scanf("%d", &columns);

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

        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < columns; j++)
            {
                global_matrix[i * columns + j] = i + 1;
            }
        }

        //Mostrar datos de la matriz global
        printf("Processor %d has data: \n", rank);
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

    // Enviando a todos los procesos la cantidad de columnas, las filas por proceso y el tamaño de las matrices
    // por cada proceso
    MPI_Bcast(&lpm_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&rows_lpm, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&columns, 1, MPI_INT, 0, MPI_COMM_WORLD);

    //Dar tamaño a la matriz local de cada proceso
    local_procc_matrix = (int *)malloc(lpm_size * sizeof(int));

    //Distribuir una cantidad igual de elementos(lpm_size) por procesos
    MPI_Scatter(global_matrix, lpm_size, MPI_INT, local_procc_matrix, lpm_size, MPI_INT, 0, MPI_COMM_WORLD);

    //Mostrar las matrices de cada proceso
    printf("Processor %d has data: ", rank);
    printf("\n");
    for (int i = 0; i < rows_lpm; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            printf("%d ", local_procc_matrix[i * columns + j]);
        }
        printf("\n");
    }

    // Liberar memoria
    free(local_procc_matrix);
    if (rank == 0)
    {
        free(global_matrix);
    }
    MPI_Finalize();
    return 0;
}