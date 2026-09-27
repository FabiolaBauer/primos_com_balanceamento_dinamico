#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

int n = 5000000;
int chunck = 5000;
int next = 1;
int total = 0;
int chunks_processados = 0;
int total_chunks;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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

void mostrar_progresso() {
    float progresso = ((float) chunks_processados / total_chunks) * 100;
    int largura = 40;
    int preenchido = (int)((progresso / 100) * largura);

    printf("\r[");
    
    for (int i = 0; i < largura; i++) {
        if (i < preenchido)
            printf("#");
        else
            printf(" ");
    }

    printf("] %.1f%%", progresso);

    if (chunks_processados == total_chunks)
    printf("\n");

    fflush(stdout);
}

void *trabalhador(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        int inicio = next;
        next += chunck;
        pthread_mutex_unlock(&mutex);

        if (inicio > n) break;

        int fim = inicio + chunck - 1;
        if (fim > n) fim = n;

        int local = 0;
        for (int i = inicio; i <= fim; i++)
            if (is_prime(i)) local++;

        pthread_mutex_lock(&mutex);

        total += local;
        chunks_processados++;
        mostrar_progresso();

        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s k\n", argv[0]);
        return 1;
    }

    int k = atoi(argv[1]);
    if (k == 0) k = sysconf(_SC_NPROCESSORS_ONLN);

    total_chunks = (n + chunck - 1) / chunck;

    pthread_t threads[k];


    struct timespec inicio, fim;
    long tempo_ms;

    clock_gettime(CLOCK_MONOTONIC, &inicio);


    for (int i = 0; i < k; i++)
        pthread_create(&threads[i], NULL, trabalhador, NULL);

    for (int i = 0; i < k; i++)
        pthread_join(threads[i], NULL);

    
    clock_gettime(CLOCK_MONOTONIC, &fim);

    tempo_ms = (fim.tv_sec - inicio.tv_sec) * 1000;
    tempo_ms += (fim.tv_nsec - inicio.tv_nsec) / 1000000;


    printf("possui %d números primos\n", total);
    printf("Tempo de execução: %ld ms\n", tempo_ms);

    return 0;
}
