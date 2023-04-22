#ifndef SRC_PLANIFICADOR_H_
#define SRC_PLANIFICADOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <send_rcvs.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include "instrucciones.h"
#include <semaphore.h>
#include <commons/collections/queue.h>

typedef struct{
	int socket;
	t_log* log;
	char* server_name;
}args_atender_cliente;

void atender_cliente(void*);
void agregarANew(pcb_t*, t_log*);

t_queue* colaNew;
t_list* listaReady;
t_list* listaExe;
t_list* listaBlock;
t_list* listaExit;

pthread_mutex_t mutexNew;
pthread_mutex_t mutexReady;
pthread_mutex_t mutexBlock;
pthread_mutex_t mutexExe;
pthread_mutex_t mutexExit;

sem_t contadorNew;
sem_t contadorReady;
sem_t contadorExe;
sem_t contadorBlock;
//sem_t contadorProcesosEnMemoria;
sem_t multiprogramacion;
//pthread_mutex_t multiprocesamiento;
sem_t largoPlazo;
#endif
