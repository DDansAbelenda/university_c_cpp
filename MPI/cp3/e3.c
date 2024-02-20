#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int comm_size, my_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (comm_size < 2)
    {
        printf("Este programa debe ejecutarse con al menos 2 procesos MPI.\n");
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    int filas, columnas;

    if (my_rank == 0)
    {
        printf("Ingrese la cantidad de filas: ");
        scanf("%d", &filas);
        printf("Ingrese la cantidad de columnas: ");
        scanf("%d", &columnas);

        // Asegúrate de que el número de filas sea múltiplo del número de procesos
        if (filas % comm_size != 0)
        {
            printf("El número de filas debe ser múltiplo de la cantidad de procesos.\n");
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
    }

    // Broadcast las dimensiones a todos los procesos
    MPI_Bcast(&filas, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&columnas, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int matriz[filas / comm_size][columnas];

    // Scatter la matriz entre los procesos
    MPI_Scatter(matriz, filas / comm_size * columnas, MPI_INT, matriz, filas / comm_size * columnas, MPI_INT, 0, MPI_COMM_WORLD);

    int sumas[filas / comm_size];
    for (int i = 0; i < filas / comm_size; i++)
    {
        sumas[i] = 0;
        for (int j = 0; j < columnas; j++)
        {
            sumas[i] += matriz[i][j];
        }
    }

    int *todas_las_sumas = NULL;

    if (my_rank == 0)
    {
        todas_las_sumas = (int *)malloc(filas * sizeof(int));
    }

    // Gather todas las sumas en el proceso 0
    MPI_Gather(sumas, filas / comm_size, MPI_INT, todas_las_sumas, filas / comm_size, MPI_INT, 0, MPI_COMM_WORLD);

    if (my_rank == 0)
    {
        printf("Sumas de cada fila:\n");
        for (int i = 0; i < filas; i++)
        {
            printf("%d ", todas_las_sumas[i]);
        }
        printf("\n");

        free(todas_las_sumas);
    }

    MPI_Finalize();
    return 0;
}
