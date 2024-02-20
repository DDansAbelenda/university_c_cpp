#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void suma(double xloc[], double y[], double z[], int n, int p, int pr);

int main(int argc, char **argv)
{
    int size, rank, n, elements_by_process;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double *xloc = NULL;
    double *y_vector = NULL;
    double *z_vector = NULL;
    double *x_vector = NULL;

    if (rank == 0)
    {
        printf("Indique la cantidad de elementos del vector, debe ser múltiplo de %d:", size);
        scanf("%d", &n);
        x_vector = (double *)malloc(n * sizeof(double));

        // Elementos por proceso
        elements_by_process = (int)(n / size);

        srand(time(NULL));

        // Llenar el vector x
        for (int i = 0; i < n; i++)
        {
            x_vector[i] = rand() % 10;
        }

        printf("El vector x es: ");
        for (int i = 0; i < n; i++)
        {
            printf("%.2f ", x_vector[i]);
        }
        printf("\n");
    }

    // Difundir los elementos comunes a todos los procesos

    MPI_Bcast(&elements_by_process, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    xloc = (double *)malloc(elements_by_process * sizeof(double));
    z_vector = (double *)malloc(elements_by_process * sizeof(double));
    y_vector = (double *)malloc(n * sizeof(double));

    if (rank == 0)
    {
        // Llenar el vector y
        for (int i = 0; i < n; i++)
        {
            y_vector[i] = rand() % 10;
        }
        printf("El vector y es: ");
        for (int i = 0; i < n; i++)
        {
            printf("%.2f ", y_vector[i]);
        }
        printf("\n");
    }

    MPI_Bcast(y_vector, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    MPI_Scatter(x_vector, elements_by_process, MPI_DOUBLE, xloc, elements_by_process, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    suma(xloc, y_vector, z_vector, n, size, rank);

    printf("El vector z del proceso %d es: ", rank);
    for (int i = 0; i < elements_by_process; i++)
    {
        printf("%.2f ", z_vector[i]);
    }
    printf("\n");

    free(xloc);
    free(y_vector);
    free(z_vector);
    free(x_vector);

    MPI_Finalize();
    return 0;
}

void suma(double xloc[], double y[], double z[], int n, int p, int pr)
{
    int elements_by_process = n / p;

    // Sumar los elementos
    for (int i = 0, w; i < elements_by_process; i++)
    {
        w = i + (elements_by_process * pr); // posicion en el vector y correspondiente al proceso actual
        z[i] = xloc[i] + y[w];
    }
}
