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
#include "kernel.h"
#include <unistd.h>

void atender_cliente(void*);
void agregarANew(pcb_t*);
void agregarABlock(pcb_t*);
pcb_t* sacarDeNew();
void hiloNew_Ready();
void hiloReady_Execute();
void hilo_Block();
pcb_t* obtener_siguiente_FIFO();
pcb_t* obtener_siguiente_ready();
pcb_t* obtener_siguiente_HRRN(time_t);
void bloquear_procesoPorIO(void* );

recurso_sistema* encontrar_recurso(t_list*,char*);
void terminarEjecucion(pcb_t* );
void agregarAReady(pcb_t*);
void sacarDeBlock(pcb_t*);
int tamanioDeListaReady();
void recalcular_rafagas_HRRN(pcb_t*, float);
void manejar_memoria(pcb_t*, uint32_t );
void manejar_contextosDeEjecucion(pcb_t*);
void manejar_recursos(pcb_t*, uint32_t ,float );
void manejar_otras_instrucciones(pcb_t*,uint32_t, float);
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
extern pthread_mutex_t multiprocesamiento;

extern sem_t contadorNew;
extern sem_t contadorReady;
extern sem_t contadorExe;
extern sem_t contadorBlock;
//sem_t contadorProcesosEnMemoria;
extern sem_t multiprogramacion;

extern sem_t largoPlazo;
#endif
