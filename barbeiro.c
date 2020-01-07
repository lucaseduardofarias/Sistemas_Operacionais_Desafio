
/*
 * Componentes do Grupo: Ariel Azzi, Lucas Eduardo Farias, Maithe Mikaele de Mello
 * Problema escolhido: O problema do Barbeiro Sonolento

*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <semaphore.h>

#define TRUE 1
#define FALSE 0
#define MAX 100

void *barbeiro(void *arg);
void *cliente(void *arg);
void corta_cabelo();
void barbeiro_cortanto();

/* Variáveis globais */
sem_t mutex, clientes, barbeiros;
int sala_espera = 0, limite_sala_espera = 5, cadeira = 1, dormindo = 0;

int main(int argc, char **argv) {
    int i, res1, res2, res3, k, m;
    char c[100];
    pthread_t barbeiro_thread, customer_thread[MAX];

    /* Inicia os semáforos */
    res1 = sem_init(&mutex, 0, 1);
    res2 = sem_init(&clientes, 0, 0);
    res3 = sem_init(&barbeiros, 0, 0);
    
    /* Verifica se os semáforos foram criados */
    if (res1 != 0 || res2 != 0 || res3 != 0) {
        perror("Falha na inicializacao do semaforo");
        exit(1);
    }
    /* Cria a thread do barbeiro */
    res1 = pthread_create(&barbeiro_thread, NULL, barbeiro, (void *)(intptr_t)1);
    if (res1 != 0) {
        perror("Falha na inicializacao do barbeiro");
        exit(1);
    }

    /* Lendo um numero de clientes que vão entrar de uma vez e os criando*/
    k = 0;
    while (TRUE) {
        fgets(c, MAX, stdin);
        m = atoi(c);
        if (m > 0) {
            for (i = k; i < m + k; i++) {
                /* Cria as thread para cliente */
                res1 = pthread_create(&customer_thread[i - k], NULL, cliente, (void *)(intptr_t)i);
                if (res1 != 0) {
                    perror("Falha na inicializacao de cliente");
                    exit(1);
                }
            }
            k = i;
        }
    }
}

/* Função do barbeiro */
void *barbeiro(void *arg) {
    /* Iniciando barbaeiro */
    printf("Barbeiro chegou para trabalhar\n");
    usleep(rand() % 10000);
    while (TRUE) {

        /* Verifica se tem clientes na sala de espera, se não inicia barbeiro dormindo */
        if (sala_espera == 0) {
            printf("Sem clientes, barbeiro durmindo na cadeira\n");
            dormindo = 1;
        }

        /* Aguarda clientes */
        sem_wait(&clientes);
        if (sala_espera == 0) {
            printf("Barbeiro acordado\n");
        }
        sem_wait(&mutex);

        /* Remove cliente da fila de espera */
        sala_espera--;

        /* Informa que o barbeiro esta cortando o cabelo */
        sem_post(&barbeiros);
        sem_post(&mutex);
        barbeiro_cortanto();
    }
}

/* Função do cliente */
void *cliente(void *arg) {
    int i = (int)(intptr_t)arg;
    usleep(rand() % 10000);
    sem_wait(&mutex);

    /* Iniciando cliente */
    printf("Cliente %d entrou\n", i);

    /* Verifica se existe lugar vago na sala de espera */
    if (sala_espera < limite_sala_espera) {

        /* Caso o barbeiro esteja dormindo, o cliente acorda ele */
        if (dormindo) {
            printf("Cliente %d acorda o barbeiro\n", i);
            dormindo = 0;
        }
        sala_espera++;

        /* Posta novo cliente */
        sem_post(&clientes);
        sem_post(&mutex);

        /* Aguarda o barbeiro */
        sem_wait(&barbeiros);

        /* Corta o cabelo do cliente */
        corta_cabelo(i);
        printf("Cliente %d cortou o cabelo\n", i);
        printf("Barbeiro vai na sala de espera\n");
    } else {
        sem_post(&mutex);
        printf("Cliente %d saiu chateado\n", i);
    }
    pthread_exit(NULL);
}

/* Função que o barbeiro corta os cabelos */
void barbeiro_cortanto() {

    /* Barbeiro cortando o cabelo */
    printf("Barbeiro cortando cabelo\n");
    usleep(rand() % 100000);
}

/* Função do cliente cortando o cabelo */
void corta_cabelo(int i) {

    /* Cliente tendo seu cabelo cortado */
    printf("Cliente %d cortando cabelo\n", i);
    usleep(rand() % 10000);
}
