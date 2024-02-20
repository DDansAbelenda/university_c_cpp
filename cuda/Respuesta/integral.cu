#include <bits/stdc++.h>
#include <chrono>

#define ThreadsPerBlock 256

// Función a integrar
__host__ __device__ double function(double x)
{
    return x * x * x; // f(x) = x^3
}

// Función para realizar una suma atómica de tipo double en el dispositivo (GPU)
__device__ double atomicAddDouble(double *address, double val)
{
    // Convierte el puntero a dirección de memoria de tipo double a un puntero de tipo unsigned long long int
    unsigned long long int *address_as_ull = (unsigned long long int *)address;

    // Lee el valor actual de la dirección de memoria y guarda el valor leído en 'old'
    unsigned long long int old = *address_as_ull, assumed;

    // Bucle do-while para realizar la suma atómica de manera segura
    do
    {
        // Asigna el valor actual de 'old' a 'assumed'
        assumed = old;

        // Utiliza atomicCAS para realizar una suma atómica en punto flotante de doble precisión
        // __double_as_longlong convierte un double a un long long int, realiza la suma y luego convierte el resultado de vuelta a double
        old = atomicCAS(address_as_ull, assumed, __double_as_longlong(val + __longlong_as_double(assumed)));

        // Continúa el bucle si el valor asumido ('assumed') no coincide con el valor actual ('old')
    } while (assumed != old);

    // Convierte el valor final en 'old' de tipo unsigned long long int a double y lo devuelve
    return __longlong_as_double(old);
}

// Kernel para el cálculo de la integral usando el método del trapecio de forma paralela
__global__ void trapecioParallel(double a, double b, double h, int n, double *d_result)
{
    // Calcula el índice global del hilo
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    // Verifica si el índice supera el número total de intervalos
    if (idx >= n)
        return;

    // Índice del hilo dentro del bloque
    const int s_idx = threadIdx.x;

    // Memoria compartida para almacenar temporalmente resultados de la función para el bloque
    __shared__ double buffer_per_block[ThreadsPerBlock];

    // Calcula el valor de x para el punto actual del intervalo
    double x = a + idx * h;

    // Evalúa la función en el punto x y almacena el resultado en memoria compartida
    buffer_per_block[s_idx] = function(x);

    __syncthreads();

    if (s_idx == 0)
    {
        double blockSum = 0;
        // Suma los resultados del bloque
        for (int i = 0; i < blockDim.x; ++i)
        {
            blockSum += buffer_per_block[i];
        }

        // Realiza una suma atómica en el resultado global
        atomicAddDouble(d_result, blockSum);
    }
}

// Función que se encarga de crear las condiciones para lanzar el kernel y lo activa
void launcherKernel(double a, double b, double h, int n)
{
    // Reservar memoria en la GPU para el resultado
    double *d_result;
    cudaMalloc((void **)&d_result, sizeof(double));
    // Inicializa el resultado en la GPU con el valor correspondiente al método del trapecio para los extremos del intervalo
    double initResult = 0.5 * (function(a) + function(b));
    cudaMemset(d_result, initResult, sizeof(double));

    // Configuración de bloques e hilos en la GPU
    int blockSize = ThreadsPerBlock;                // hilos por bloque
    int gridSize = (n + blockSize - 1) / blockSize; // bloques por grid

    // Lanzar el kernel en la GPU
    trapecioParallel<<<gridSize, blockSize>>>(a, b, h, n, d_result);

    // Copiar el resultado desde la GPU a la CPU
    double result;
    cudaMemcpy(&result, d_result, sizeof(double), cudaMemcpyDeviceToHost);

    // Imprimir el resultado
    printf("Resultado de la integral: %f \n", result * h);
    // Liberar memoria en la GPU
    cudaFree(d_result);
}

// Función para el cálculo de la integral usando el método del trapecio de forma secuencial
void trapecio(double a, double b, double h, int n)
{

    double result = 0.5 * (function(a) + function(b));

    for (int i = 1; i < n; i++)
    {

        double x = a + i * h;

        result += function(x);
    }
}

int main(int argc, char *argv[])
{
    // Verificar los parámetros
    if (argc != 4)
    {
        std::cerr << "Uso: " << argv[0] << " <a> <b> <n_intervalos>" << std::endl;
        return 1;
    }

    // Inicialización de los intervalos y la cantidad de trapecios
    double a = atof(argv[1]);
    double b = atof(argv[2]);
    int n = atoi(argv[3]);
    double h = (b - a) / n; // calcula el ancho del intervalo

    // Trabajo en Paralelo
    printf("Calculo en Paralelo\n");

    //  Crear eventos para medir el tiempo
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    // Registrar el evento de inicio
    cudaEventRecord(start, 0);

    // Lanzar el cálculo paralelo
    launcherKernel(a, b, h, n);

    // Registrar el evento de parada
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);

    // Calcular y imprimir el tiempo de ejecución
    float parallelTime = 0;
    cudaEventElapsedTime(&parallelTime, start, stop);
    printf("Tiempo de ejecución: %f ms\n", parallelTime);

    // Dividir
    printf("--------------------------\n");

    // Trabajo secuencial
    printf("Calculo Secuencial\n");

    // Medir el tiempo de ejecución
    clock_t init_time = clock();

    // Llamar a la función
    trapecio(a, b, h, n);

    // Calcular e imprimir el tiempo transcurrido en milisegundos
    clock_t finish_time = clock();
    float sequentialTime = (float)(finish_time - init_time) / CLOCKS_PER_SEC;

    printf("Tiempo de ejecución: %f ms\n", sequentialTime * 1000);

    printf("----------------------\n");

    printf("Para N = %d el speedup es: %f \n", n, (sequentialTime * 1000) / parallelTime);

    return 0;
}
