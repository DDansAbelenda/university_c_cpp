#!/bin/bash
#SBATCH -p public # El job se envia a la cola public
#SBATCH -J MPItest # Nombre del job 'MPItest'
#SBATCH -o MPItest-%j.out # La salida standard se escribe hacia el fichero 'MPItest-<job_number>.out'
#SBATCH -e MPItest-%j.err # La salida de error se escribe hacia el fichero'MPItest-<job_number>.err'
#SBATCH -t 0-12:00:00 # Tiempo máximo de ejcucion 0 dias, 12 horas, 00 mins, 00 secs
#SBATCH --nodes=2 # Cantidad de nodos
#SBATCH --ntasks-per-node=1 # Cantidad de tareas por nodo
#sbatch --cpus-per-task=1 # Cantidad de cores por tarea
#SBATCH --mem-per-cpu=4GB # Request 4GB RAM per core
#SBATCH --mail-type=BEGIN,END,FAIL,ALL # Notificacion por email
#SBATCH --mail-user=ddans@uclv.cu # email a notificar
module load OpenMPI
mpirun ex_3b 8 4