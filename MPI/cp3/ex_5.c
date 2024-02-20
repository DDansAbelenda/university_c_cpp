#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv)
{
    int size, rank, N, i, elements_by_process;
    int *V, *Subv, sum = 0, process_sum = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        // printf("Longitud del vector, debe ser multiplo de %d: ", size);
        // scanf("%d", &N);
        N = 1048576;
        elements_by_process = N / size;

        V = (int *)malloc(sizeof(int) * N);

        // Cambiar semilla
        srand(time(NULL));
        for (i = 0; i < N; i++)
        {
            V[i] = rand() % 100 - 50; // inicializacion del vector
        }
    }

    // Difundir la cantidad de elementos por proceso
    MPI_Bcast(&elements_by_process, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Procesamiento del vector
    Subv = (int *)malloc(elements_by_process * sizeof(int));

    // Enviar a cada proceso una parte del vector V
    MPI_Scatter(V, elements_by_process, MPI_INT, Subv, elements_by_process, MPI_INT, 0, MPI_COMM_WORLD);

    // Sumas parciales
    for (i = 0; i < elements_by_process; i++)
    {
        process_sum += Subv[i];
    }

    // Enviar la suma total a cada proceso
    MPI_Allreduce(&process_sum, &sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    // Cada proceso multiplica la suma total por el subvector correspondiente
    for (i = 0; i < elements_by_process; i++)
    {
        Subv[i] = Subv[i] * sum;
    }

    // Luego se reciben esos subvectores modificados
    MPI_Gather(Subv, elements_by_process, MPI_INT, V, elements_by_process, MPI_INT, 0, MPI_COMM_WORLD);

    free(Subv);

    if (rank == 0)
    {
        // Imprimir unos resultados
        printf("\n sum = % d\n", sum);
        printf("\n V[0] = %d, V[N / 2] = %d, V[N - 1] = %d \n\n\n", V[0], V[N / 2], V[N - 1]);
        free(V);
    }
    MPI_Finalize();
    return 0;
}
