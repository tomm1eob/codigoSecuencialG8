#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAREA 1
#define RESULTADO 2
#define FIN 3
#define ULTIMA 4
#define MAX_CACHESIZE 101  // Limitar la caché a números del 0 al 100

int esPrimo(int num, int *cache) {
    // Si el número está en el rango de la caché, usa el valor almacenado
    if (num < MAX_CACHESIZE) {
        if (cache[num] != -1) {
            return cache[num];  // Usa el resultado almacenado
        }
    }

    // Cálculo del número primo
    if (num <= 1) return 0;
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) {
            if (num < MAX_CACHESIZE) cache[num] = 0;  // Almacenar en la caché
            return 0;
        }
    }
    if (num < MAX_CACHESIZE) cache[num] = 1;  // Almacenar en la caché
    return 1;
}

void inicializar(int *matriz, int filas, int columnas) {
    srand(time(NULL));
    for (int i = 0; i < (filas * columnas); i++) {
        matriz[i] = rand() % 101;  // Valores aleatorios entre 0 y 100
    }
}

// (El resto de tus funciones como mostrar_matriz e inicialización siguen igual)

int main(int argc, char **argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        printf("[!] Se necesitan al menos 2 procesos\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int filas = 5000;
    int columnas = 5000;
    int cantFilas = 1000;

    if (rank == 0) {
        // Master
        printf("[+] Master: cantidad de Workers %d\n", size - 1);
        printf("[+] Master: matriz de %d x %d a %d fila/worker\n", filas, columnas, cantFilas);
        double inicio, fin;
        int primos_totales = 0;
        int worker = 1;
        int resto = filas % cantFilas;
        MPI_Status status;
        int *matriz = (int *)malloc(filas * columnas * sizeof(int));

        inicializar(matriz, filas, columnas);
        inicio = MPI_Wtime();

        for (int i = 0; i < (filas * columnas); i += (cantFilas * columnas)) {
            if (worker < size) {
                MPI_Send(&matriz[i], columnas * cantFilas, MPI_INT, worker, TAREA, MPI_COMM_WORLD);
                worker++;
            } else {
                int primos;
                MPI_Recv(&primos, 1, MPI_INT, MPI_ANY_SOURCE, RESULTADO, MPI_COMM_WORLD, &status);
                primos_totales += primos;

                if (resto != 0 && i == (filas - resto) * columnas) {
                    MPI_Send(&matriz[i], columnas * resto, MPI_INT, status.MPI_SOURCE, ULTIMA, MPI_COMM_WORLD);
                    MPI_Send(&resto, 1, MPI_INT, status.MPI_SOURCE, ULTIMA, MPI_COMM_WORLD);
                    break;
                }
                MPI_Send(&matriz[i], columnas * cantFilas, MPI_INT, status.MPI_SOURCE, TAREA, MPI_COMM_WORLD);
            }
        }

        for (int i = 1; i < size; i++) {
            int primos;
            MPI_Recv(&primos, 1, MPI_INT, MPI_ANY_SOURCE, RESULTADO, MPI_COMM_WORLD, &status);
            primos_totales += primos;

            MPI_Send(NULL, 0, MPI_INT, status.MPI_SOURCE, FIN, MPI_COMM_WORLD);
        }

        fin = MPI_Wtime();
        printf("[>] Número total de primos: %d\n", primos_totales);
        printf("[>] Tiempo: %f segundos\n", fin - inicio);

        free(matriz);
        printf("[*] Master terminado\n\n");

    } else {
        // Workers
        MPI_Status status;
        int *fila = (int *)malloc(columnas * cantFilas * sizeof(int));
        int cache[MAX_CACHESIZE];
        for (int i = 0; i < MAX_CACHESIZE; i++) cache[i] = -1;  // Inicializar caché con -1

        int primos_locales;
        while (1) {
            MPI_Recv(fila, columnas * cantFilas, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if (status.MPI_TAG == TAREA) {
                primos_locales = 0;
                for (int i = 0; i < columnas * cantFilas; i++) {
                    primos_locales += esPrimo(fila[i], cache);
                }
                MPI_Send(&primos_locales, 1, MPI_INT, 0, RESULTADO, MPI_COMM_WORLD);

            } else if (status.MPI_TAG == ULTIMA) {
                int resto;
                MPI_Recv(&resto, 1, MPI_INT, 0, ULTIMA, MPI_COMM_WORLD, &status);
                primos_locales = 0;
                for (int i = 0; i < columnas * resto; i++) {
                    primos_locales += esPrimo(fila[i], cache);
                }
                MPI_Send(&primos_locales, 1, MPI_INT, 0, RESULTADO, MPI_COMM_WORLD);
            } else if (status.MPI_TAG == FIN) {
                printf("[*] Worker %d terminado\n", rank);
                break;
            }
        }
        free(fila);
    }
    MPI_Finalize();
    return 0;
}
