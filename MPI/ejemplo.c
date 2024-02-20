#include <mpi.h> 
#include <stdio.h>
int main(int argc, char *argv[])
{
int np, p;
MPI_Init( &argc, &argv ); 
MPI_Comm_size(MPI_COMM_WORLD, &np );  
MPI_Comm_rank( MPI_COMM_WORLD, &p );  
printf("Soy el proceso %i de un total de %i \n", p, np );
MPI_Finalize();
}
