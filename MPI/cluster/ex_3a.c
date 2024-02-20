#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int remainder_fun(int *array_to_full, int remainder, int mult);

int main(int argc, char **argv)
{
    int size, rank, rows, columns, matrix_size, number_rows_dist;
    double start, end;

    MPI_Init(&argc, &argv);
    start = MPI_Wtime(); // Tiempo inicio
    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int *global_matrix = NULL;      // Matriz global
    int *sum_total_matrix = NULL;   // Vector que almacena todas las sumas por fila
    int *local_procc_matrix = NULL; // Matriz de cada proceso
    int *lpm_sum = NULL;            // vector que almacena las sumas por proceso
    int *sendcounts = NULL;         // vector de enteros que especifica cuántos elementos se envia a cada proceso
    int *displs = NULL;             // vector de enteros que especifica el desplazamiento en el buffer de envío desde
                                    // donde se tomarán los datos para enviar a cada proceso.
    int *recvcounts = NULL;         // vector de enteros que especifica cuántos elementos se van a recibir de cada proceso
    int *recvdisp = NULL;           // vector de enteros que especifica el desplazamiento en el buffer de recepción donde
                                    // se almacenarán los datos de cada proceso.
    if (rank == 0)
    {
        // Recibir las filas
        rows = atoi(argv[1]);
        // Recibir las columnas
        columns = atoi(argv[2]);

        // Verificar si la cantidad de argumentos es correcta
        if (argc != 3)
        {
            fflush(stdout);
            MPI_Finalize();
            return 0;
        }       
        // Tamaño de la matriz
        matrix_size = rows * columns;

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
        printf("La matriz que se genera es: \n");
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

    // Enviando a todos los procesos la cantidad de columnas, las filas por proceso y el tamaño de las matrices
    // por cada proceso
    MPI_Bcast(&columns, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);

    /* Calculando los vectores de parametros de Scatterv */
    
    // Calculando la cantidad de filas por proceso
    sendcounts = (int *)malloc(size * sizeof(int));
    displs = (int *)malloc(size * sizeof(int));

    number_rows_dist = (int)(rows / size);

    // Si desde un inicio la cantidad de filas es menor que la cantidad de procesos repartir con remainder_fun
    // 1 fila * columnas a cada proceso hasta que se acaben, en este caso hay procesos que se quedan sin filas
    if (number_rows_dist == 0)
    {
        remainder_fun(sendcounts, rows, columns);
    }
    else
    {
        // De lo contrario repartimos cantidades iguales, en number_rows_count se almacena la cantidad de filas
        // por proceso y estas se ponene en sendcounts para cada proceso multiplicada por las columnas de la tabla
        // lo que da el total de elementos con los que trabaja un proceso
        for (int i = 0; i < size; i++)
        {
            sendcounts[i] = number_rows_dist * columns;
        }
        // En caso de que la cantidad de filas es mayor que la cantidad de procesos pero 
        // no es multiplo, se completa repartiendo los restantes elementos entre los procesos
        if (rows % size != 0)
        {
            remainder_fun(sendcounts, rows % size, columns);
        }
    }
    // Calcular los desplazamientos
    displs[0] = 0;
    for (int i = 1; i < size; i++)
    {
        displs[i] = displs[i - 1] + sendcounts[i - 1];
    }

    // Dar tamaño a la matriz local de cada proceso
    local_procc_matrix = (int *)malloc(sendcounts[rank] * sizeof(int));

    // Dar tamaño a la matriz de suma local de cada proceso
    lpm_sum = (int *)malloc((sendcounts[rank] / columns) * sizeof(int));

    // Distribuir la cantidad de elementos por proceso
    MPI_Scatterv(global_matrix, sendcounts, displs, MPI_INT, local_procc_matrix, sendcounts[rank], MPI_INT, 0, MPI_COMM_WORLD);

    
    // Sumar las filas de la matriz
    for (int i = 0; i < (sendcounts[rank] / columns); i++)
    {
        lpm_sum[i] = 0;
        for (int j = 0; j < columns; j++)
        {
            lpm_sum[i] += local_procc_matrix[i * columns + j];
        }
    }
    
    // Preparar los arreglos para el Scatterv
    if (rank == 0)
    {
        sum_total_matrix = (int *)malloc(rows * sizeof(int));
        recvcounts = (int *)malloc(size * sizeof(int));
        recvdisp = (int *)malloc(size * sizeof(int));
        for (int i = 0; i < size; i++)
        {
            recvcounts[i] = sendcounts[i] / columns;
        }
        recvdisp[0] = 0;
        for (int i = 1; i < size; i++)
        {
            recvdisp[i] = recvdisp[i - 1] + recvcounts[i - 1];
        }
    }

    MPI_Gatherv(lpm_sum, (sendcounts[rank] / columns), MPI_INT, sum_total_matrix, recvcounts, recvdisp, MPI_INT, 0, MPI_COMM_WORLD);

        // Liberar memoria
    free(local_procc_matrix);
    free(lpm_sum);

    // Mostrar la matriz de las sumas global
    if (rank == 0)
    {
        printf("El vector de sumas de la matriz es: ");
        for (int i = 0; i < rows; i++)
        {
            printf("%d ", sum_total_matrix[i]);
        }
        printf("\n");

        //Liberar memoria
        free(sum_total_matrix);
        free(global_matrix);
        free(displs);
        free(sendcounts);
        
        end = MPI_Wtime(); // Tiempo final
        printf("Tiempo ejecución: %f", end - start);
    }


    MPI_Finalize();
    return 0;
}

// Esta funcion adiciona mult a cada elemento  del arreglo segun el tamaño de remainder,
// es decir, segun el resto de la division entre el total de filas y la cantidad de procesos
int remainder_fun(int *array_to_full, int remainder, int mult)
{
    while (remainder)
    {
        int i = 0;
        array_to_full[i++] += mult;
        remainder--;
    }
}