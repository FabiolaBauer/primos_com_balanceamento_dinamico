#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

bool is_even(int n) {
    return n % 2 == 0 && n != 2;
}

bool is_prime(int n) {
    if (n <= 1) return false;
    if (is_even(n)) return false;

    for (int i = 3; i * i <= n; i++) {
        if (n % i == 0) return false;
    }
    return true;
}

int loop_numeros(int n, int chunck) {
    int contador = 0;
    int largura = 40;

    for (int i = 1; i < n; i++) {
        if (is_prime(i)) contador++;

        if (i % chunck == 0) {
            float proporcao = (float)i / n;
            int preenchido = (int)(proporcao * largura);

            printf("\r[");
            for (int j = 0; j < largura; j++) {
                if (j < preenchido)
                    printf("#");
                else
                    printf(" ");
            }
            printf("] %.1f%%", proporcao * 100);
            fflush(stdout);
        }
    }
    printf("\n");

    return contador;
}
int main() {
    int n = 5000000;
    int chunck = 5000;

    clock_t inicio, fim;
    double tempo_gasto;
    inicio = clock();

    int resultado = loop_numeros(n, chunck);

    fim = clock();
    tempo_gasto = ((double) (fim - inicio)) / CLOCKS_PER_SEC;

    printf("possui %d números primos\n", resultado);
    printf("Tempo de execução: %f segundos\n", tempo_gasto);
    
    return 0;
}