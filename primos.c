#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

int n = 5000000;
int chunck = 5000;
int next = 1;
int total = 0;
int chunks_processados = 0;
int total_chunks;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int benchmark_cases[] = {1,2,4,6,8}; 

typedef struct {
    int k;
    long tempo_ms;
    int total_primos;
    float speedup_vs_k1;
} BenchmarkMetrica;

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

long makeThreads(int k) {
    if (k == 0) k = sysconf(_SC_NPROCESSORS_ONLN);
    next = 1;
    total = 0;
    chunks_processados = 0;

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

    return tempo_ms;
    
}

void exibeTabela(BenchmarkMetrica *metrica, int cases) {
    printf("\n%-10s %-20s %-30s %-15s\n", "K", "tempo_ms", "Total de primos", "speedup_vs_k1");
    for (int i = 0; i < cases; i ++) {
        printf("%-10d %-20ld %-30d %-15.3f\n", metrica[i].k, metrica[i].tempo_ms, metrica[i].total_primos, metrica[i].speedup_vs_k1);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s k\n", argv[0]);
        return 1;
    }

    if(strcmp(argv[1], "b") == 0) {
        int casos = sizeof(benchmark_cases) / sizeof(benchmark_cases[0]);
        BenchmarkMetrica metrica[casos];

        for(int i = 0; i < casos; i++) {
            int k = benchmark_cases[i];
            long tempo_ms = makeThreads(k);
            metrica[i].k = k;
            metrica[i].tempo_ms = tempo_ms;
            metrica[i].total_primos = total;
            metrica[i].speedup_vs_k1 = (float)metrica[0].tempo_ms/metrica[i].tempo_ms;
        }

        exibeTabela(metrica, casos);

    } else {
        int k = atoi(argv[1]);
        long tempo_ms = makeThreads(k);
        printf("possui %d números primos\n", total);
        printf("Tempo de execução: %ld ms\n", tempo_ms);
    }
    
    return 0;
}
