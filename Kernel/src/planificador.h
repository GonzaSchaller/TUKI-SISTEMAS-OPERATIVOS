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
#include "conexiones_kernel.h"

void atender_cliente(void*);
void agregarANew(pcb_t*, t_log*);

//para poder usarlas en el main
extern t_queue* colaNew;
extern t_list* listaReady;
extern t_list* listaExe;
extern t_list* listaBlock;
extern t_list* listaExit;

extern pthread_mutex_t mutexNew;
extern pthread_mutex_t mutexReady;
extern pthread_mutex_t mutexBlock;
extern pthread_mutex_t mutexExe;
extern pthread_mutex_t mutexExit;

extern sem_t contadorNew;
extern sem_t contadorReady;
extern sem_t contadorExe;
extern sem_t contadorBlock;
//sem_t contadorProcesosEnMemoria;
extern sem_t multiprogramacion;
//pthread_mutex_t multiprocesamiento;
extern sem_t largoPlazo;
#endif
