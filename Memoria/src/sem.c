// mutex y semaforos
#include "sem.h"
pthread_mutex_t mutex_segmentos_libres;
pthread_mutex_t mutex_segmentos_usados;

pthread_mutex_t mutex_memoria_ocupada;

pthread_mutex_t mutex_compactacion;

pthread_mutex_t mutex_segm_0;

//sem_t sem_cant_segmentos;


//los inicio.
void iniciar_mutex() {
    pthread_mutex_init(&mutex_segmentos_libres, NULL);
    pthread_mutex_init(&mutex_segmentos_usados, NULL);
    pthread_mutex_init(&mutex_memoria_ocupada, NULL);
    pthread_mutex_init(&mutex_compactacion, NULL);
    pthread_mutex_init(&mutex_segm_0, NULL);

 //   sem_init(&sem_cant_segmentos, 0, 0);

}

//los destruyo.
void finalizar_mutex() {
    pthread_mutex_destroy(&mutex_segmentos_libres);
    pthread_mutex_destroy(&mutex_segmentos_usados);
    pthread_mutex_destroy(&mutex_memoria_ocupada);
    pthread_mutex_destroy(&mutex_compactacion);
    pthread_mutex_destroy(&mutex_segm_0);


  //  sem_destroy(&sem_cant_segmentos);

}
