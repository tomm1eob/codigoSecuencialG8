#include <stdio.h>    // Biblioteca estándar para entrada y salida, como printf.
#include <stdlib.h>   // Biblioteca estándar para funciones de gestión de memoria, como malloc y atoi.
#include <time.h>     // Biblioteca para manipulación de tiempo, como clock y time.

// Función que verifica si un número es primo.
int esPrimo(int num) {
    if (num <= 1) return 0; // Si el número es 1 o menor, no es primo.
    for (int i = 2; i * i <= num; i++) { // Itera hasta la raíz cuadrada del número.
        if (num % i == 0) return 0; // Si es divisible por 'i', no es primo.
    }
    return 1; // Si pasa todos los chequeos, es primo.
}

// Función que inicializa la matriz con valores aleatorios entre 0 y 100.
void inicializar(int **matriz, int fila, int columna) {
    srand(time(NULL)); // Inicializa la semilla de números aleatorios usando el tiempo actual.
    for (int i = 0; i < fila; i++) { // Itera sobre las filas de la matriz.
        for (int j = 0; j < columna; j++) { // Itera sobre las columnas de la matriz.
            matriz[i][j] = rand() % 101; // Asigna un número aleatorio entre 0 y 100 a cada posición.
        }
    }
}

// Función que muestra la matriz en pantalla.
void mostrarRand(int **matriz, int fila, int columna) {
    for (int i = 0; i < fila; i++) { // Itera sobre las filas de la matriz.
        for (int j = 0; j < columna; j++) { // Itera sobre las columnas de la matriz.
            printf("%d ", matriz[i][j]); // Imprime el valor en la posición (i, j).
        }
        printf("\n"); // Salta a la siguiente línea al final de cada fila.
    }
}

// Función que cuenta la cantidad de números primos en la matriz.
void calcularRand(int **matriz, int fila, int columna) {
    int contador = 0; // Inicializa el contador de números primos.
    for (int i = 0; i < fila; i++) { // Itera sobre las filas de la matriz.
        for (int j = 0; j < columna; j++) { // Itera sobre las columnas de la matriz.
            contador += esPrimo(matriz[i][j]); // Suma 1 al contador si el número es primo.
        }
    }
    printf("Cantidad de numeros primos: %d\n", contador); // Imprime la cantidad de números primos encontrados.
}

// Función principal, recibe el número de filas y columnas como argumentos de la línea de comandos.
int main(int argc, char *argv[]) {
    // Verifica que haya suficientes argumentos en la línea de comandos.
    if (argc < 3) {
        printf("Uso: %s <filas> <columnas>\n", argv[0]);
        return 1; // Termina el programa si no hay suficientes argumentos.
    }

    int fila = atoi(argv[1]); // Convierte el primer argumento a entero para el número de filas.
    int columna = atoi(argv[2]); // Convierte el segundo argumento a entero para el número de columnas.

    // Reserva memoria para la matriz de punteros.
    int **matriz = (int **)malloc(fila * sizeof(int *));
    for (int i = 0; i < fila; i++) {
        matriz[i] = (int *)malloc(columna * sizeof(int)); // Reserva memoria para cada fila.
    }

    inicializar(matriz, fila, columna); // Llama a la función para inicializar la matriz.
    mostrarRand(matriz, fila, columna); // Llama a la función para mostrar la matriz.

    clock_t inicio, fin; // Variables para medir el tiempo de ejecución.
    double tiempo; // Variable para almacenar el tiempo en segundos.
    inicio = clock(); // Captura el tiempo de inicio.
    calcularRand(matriz, fila, columna); // Llama a la función para calcular la cantidad de números primos.
    fin = clock(); // Captura el tiempo de finalización.
    tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC; // Calcula el tiempo transcurrido en segundos.
    printf("Tiempo: %f segundos\n", tiempo); // Imprime el tiempo de ejecución.

    // Libera la memoria asignada para la matriz.
    for (int i = 0; i < fila; i++) {
        free(matriz[i]); // Libera cada fila.
    }
    free(matriz); // Libera el array de punteros.

    return 0; // Indica que el programa finalizó correctamente.
}
