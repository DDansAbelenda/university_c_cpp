//
// Created by yadier on 26/09/23.
//
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "time.h"



int main(int argc, char** argv) {
    int rank, size, n;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("Introduce el tamano del arreglo");
        scanf("%d", &n);
        int arr[n];
        for (int i = 0; i < n; ++i) {
            arr[i] = rand();
        }
        MPI_Send(&arr, n, MPI_INT, 1, 7, MPI_COMM_WORLD);
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank != 0) {
        int arr[n];
        MPI_Recv(&arr, n, MPI_INT, rank - 1,  7, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Soy el proceso %d y recibo este arreglo para luego enviarlo\n", rank);
        for (int i = 0; i < n; ++i) {
            printf("%d ,", arr[i]);
        }
        //Espacio en Blanco
        printf("\n");
        MPI_Send(&arr, n, MPI_INT, (rank + 1) % size, 7, MPI_COMM_WORLD);
    } else {
        int arr[n];
        MPI_Recv(&arr, n, MPI_INT, size - 1,  7, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Soy el proceso %d y recibo este arreglo para terminar\n", rank);
        for (int i = 0; i < n; ++i) {
            printf("%d ,", arr[i]);
        }
        //Espacio en Blanco
        printf("\n");
    }

    MPI_Finalize();

    return 0;
}
