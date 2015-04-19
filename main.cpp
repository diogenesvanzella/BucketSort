/* 
 * File:   main.cpp
 * Authors: Diogenes Vanzella / Luiz Felipe
 *
 * Created on 12 de Abril de 2015, 16:46
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

//Pra usar a função rand(), temos que adicionar a biblioteca time.h 
//e para saber o valor de RAND_MAX, temos que usar a função stdlib.h.

// constantes para ser usado na execução do programa
#define tamvet 50  // quantidade de elementos a ser gerado
#define nbuckets 5  // quantidade de buckets para separar o vetor original
#define nthreads 4  // quantidade de Threads para executar o programa

//cria uma estrutura contendo 2 elementos do tipo inteiro e um vetor de int
//a estrutura representará um novo tipo de dados, chamado "bucket"
//logo, será possível criar variáveis do tipo "bucket"

typedef struct {
    int id;
    int tam;
    int elementosVetor[tamvet];
} bucket;

// Variaveis globias usadas na execução do programa
int MINIMO = -1; // contem o menor valor da faixa do bucket
int alternaBucket = 0; // usado pela função "thread_bucket", para alternar entre os buckets
int compBucket = 0; // usado pela função "thread_bucket", para verificar o numero de buckets
double faixaNumeroBuckets = (tamvet / nbuckets);
pthread_mutex_t mutex;
bucket *vetorBucket;

//método que cria um vetor desordenado de ordem tam;

void bubble_sort(int *v, int tam) {
    int i, j, temp, trocou;
    for (j = 0; j < tam - 1; j++) {
        trocou = 0;
        for (i = 0; i < tam - 1; i++) {
            if (v[i + 1] < v[i]) {
                temp = v[i];
                v[i] = v[i + 1];
                v[i + 1] = temp;
                trocou = 1;
            }
        }
        if (!trocou) break;
    }
}

// recebe um ponteiro que aponta para o primeiro endereço de memória do vetor
// asssim podemos manipula-lo como se fosse um vetor normal

void cria_bucktes(int *pVetorOriginal) {

    // aloca dinamicamente um vetor de tamanho "nbuckets"
    vetorBucket = (bucket *) malloc(sizeof (bucket) * nbuckets);
    int j = 0; // usado como iterador para os elementos de cada bucket
               //e ao final de cada loopint terá o tamanho do vetor
    int valorMaior = 0; // variavel auxiliar que conterá o maior valor da faixa de cada bucket
    int valorMenor = 0; // variavel auxiliar que conterá o menor valor da faixa de cada bucket

    // verifica se a divisão de buckets por quantidade de elementos é inteira
    if ((tamvet % nbuckets) != 0) {

        // buckets que terão +1 em relação numero médio
        int quaBucketsPrimarios = (tamvet % nbuckets); // pega o resultado da divisão inteira
        int faixaBucketsPrimarios = (tamvet / nbuckets) + 1; // pega quantidade de valores que o bucket vai conter

        // buckets que terão -1 em relação aos BucktesPrimarios
        int quaBucketsSecundarios = nbuckets - (tamvet % nbuckets);  // pega o resto da divisão
        int faixaBucketsSecundarios = (tamvet / nbuckets);  // pega quantidade de valores que o bucket vai conter

        // para preencher os bucktes+1
        for (int i = 0; i < quaBucketsPrimarios; ++i) {
            //usa "i" como identificador de cada bucket
            vetorBucket[i].id = i;
            // Como MINIMO é -1 o primeiro vetor recebe valor minimo 0
            valorMenor = MINIMO + 1;
            // MINIMO recebe agora ele mesmo mais a faixa de números. Ex.: faixa = 10 então agora MINIMO = 9
            MINIMO = MINIMO + faixaBucketsPrimarios;
            // Como MINIMO = 9 valor maior do primeiro vetor recebe 9
            valorMaior = MINIMO;
            // Para atribuir os valores a cada posição do bucket
            for (int k = 0; k < tamvet; ++k) {
                if ((pVetorOriginal[k] >= valorMenor) && (pVetorOriginal[k] <= valorMaior)) {
                    vetorBucket[i].elementosVetor[j] = pVetorOriginal[k];
                    j++;
                }
            }
            // Zera j para poder começar a atribuir na posição inicial dos buckets seguintes.
            vetorBucket[i].tam = j;// j contem a quantidade de valores no bucket atual
            j = 0;
        }
        
        // para preencher os bucktes-1
        for (int i = 0; i < quaBucketsSecundarios; ++i) {
            vetorBucket[quaBucketsPrimarios].id = quaBucketsPrimarios;
            // Como MINIMO é -1 o primeiro vetor recebe valor minimo 0
            valorMenor = MINIMO + 1;
            // MINIMO recebe agora ele mesmo mais a faixa de números. Ex.: faixa = 10 então agora MINIMO = 9
            MINIMO = MINIMO + faixaBucketsSecundarios;
            // Como MINIMO = 9 valor maior do primeiro vetor recebe 9
            valorMaior = MINIMO;
            // Para atribuir os valores a cada posição do bucket
            for (int k = 0; k < tamvet; ++k) {
                if ((pVetorOriginal[k] >= valorMenor) && (pVetorOriginal[k] <= valorMaior)) {
                    vetorBucket[quaBucketsPrimarios].elementosVetor[j] = pVetorOriginal[k];
                    j++;
                }
            }
            // Zera j para poder começar a atribuir na posição inicial dos buckets seguintes.
            vetorBucket[quaBucketsPrimarios].tam = j; // j contem a quantidade de valores no bucket atual
            quaBucketsPrimarios++;
            j = 0;
        }

    // caso for divisão inteira
    } else {
        // Para alternar de buckets
        for (int i = 0; i < nbuckets; ++i) {
            vetorBucket[i].id = i;
            // Como MINIMO é -1 o primeiro vetor recebe valor minimo 0
            valorMenor = MINIMO + 1;
            // MINIMO recebe agora ele mesmo mais a faixa de números. Ex.: faixa = 10 então agora MINIMO = 9
            MINIMO = MINIMO + faixaNumeroBuckets;
            // Como MINIMO = 9 valor maior do primeiro vetor recebe 9
            valorMaior = MINIMO;
            // Para atribuir os valores a cada posição do bucket
            for (int k = 0; k < tamvet; ++k) {
                if ((pVetorOriginal[k] >= valorMenor) && (pVetorOriginal[k] <= valorMaior)) {
                    vetorBucket[i].elementosVetor[j] = pVetorOriginal[k];
                    j++;
                }
            }
            // Zera j para poder começar a atribuir na posição inicial dos buckets seguintes.
            vetorBucket[i].tam = j; // j contem a quantidade de valores no bucket atual
            j = 0;
        }
    }
}

void *thread_bucket() {
    // enquanto todos os buckets não forem ordenados
    while (compBucket < nbuckets) {
        pthread_mutex_lock(&mutex); // tratando condição de corrida
        // condição usada para mandar a thread para ou voltar pra lista de execução
        if (compBucket < nbuckets) {
            bubble_sort(vetorBucket[alternaBucket].elementosVetor, vetorBucket[alternaBucket].tam);
            pthread_t db = pthread_self();
            printf("Thread %d processando bucket %d \n", ((unsigned int) db - 1), vetorBucket[alternaBucket].id);
            alternaBucket++;
            compBucket++;
            pthread_mutex_unlock(&mutex);
        } else {
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        }
    }
}

int main(int argc, char **argv) {
    int i;

    //recupera o numero de threads passado na linha de comando: ex. ./programa 5
    //os parâmetros passados na linha de comando são strings, logo é necessário converter
    //para o tipo de dados desejado. Nesse caso o valor é inteiro, logo utiliza-se atoi()
    int n_threads = atoi(argv[1]);

    //cria o vetor do tipo pthread_t para armazenar o retorno de pthread_create() de cada thread
    //o número de elementos do vetor será igual ao número de threads passado na linha de comando (n_threads)
    pthread_t *threads = (pthread_t *) malloc(sizeof (pthread_t) * n_threads);

    //
    bucket *id = (bucket *) malloc(sizeof (bucket) * nbuckets);

    //aloca dinamicamente um vetor que armazenará os parametros passados para cada thread
    //o vetor terá tamanho n_threads
    //cada parâmetro será do tipo "parametros_thread_t", ou seja, conterá 2 elementos inteiros que serão passados a cada thread
    parametros_thread_t *params = (parametros_thread_t *) malloc(sizeof (parametros_thread_t) * n_threads);

    //preenche os parâmetros a serem passados a cada thread e cria as threads
    for (i = 0; i < n_threads; i++) {
        params[i].parametro1 = i;
        params[i].parametro2 = i + 1;
        pthread_create(&threads[i], NULL, thread, (void *) &params[i]);
    }

    //aguarda a finalização de todas as threads
    for (i = 0; i < n_threads; i++)
        pthread_join(threads[i], NULL);

    //libera o espaço alocado para os vetores de threads e parâmetros
    free(threads);
    free(params);

    return 0;
}
