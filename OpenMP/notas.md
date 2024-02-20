# Notas sobre OpenMP

### Comando para compilar
```bash
gcc -o programa programa.c -fopenmp
```

### Código

*Iniciar una sección paralela*

```c
#pragma omp parallel 
{
    //El código de esta sección se ejecuta en paralelo
}
```
*Clausura asociadas a la sección paralela*

`shared(<variables>)`: define las variables que serán compartidas entre los diferentes procesos, esta cláusura no puede ser usada en el constructor for

*Constructor for*
```c
#pragma omp for
```
El for por defecto utiliza `schedule(static)` si no se define

*Cantidad de hilos*
```c
omp_get_num_threads(); //devuelve la cantidad de hilos
mp_get_thread_num(); //devuelve el hilo actual
```
