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
#include <unistd.h>
#include "kernel.h"

typedef struct{
	pcb_t* pcb;
	fcb_kernel* archivo;
	uint32_t nombre_instruccion;
}arg_archivo_bloqueado;


void atender_cliente(void*);
void agregarANew(pcb_t*);
void agregarABlock(pcb_t*);
void agregarNewAReady(pcb_t*);
void hiloNew_Ready();
void hiloReady_Execute();
void hilo_Block();
void terminarEjecucion(pcb_t* );
void agregarAReady(pcb_t*);
void sacarDeBlock(pcb_t*);
void recalcular_rafagas_HRRN(pcb_t*, float);
void print_lista_PID();
void liberar_Recursos(pcb_t*);

pcb_t* obtener_siguiente_FIFO();
pcb_t* obtener_siguiente_ready();
pcb_t* obtener_siguiente_HRRN(time_t);
pcb_t* sacarDeNew();

int tamanioDeListaReady();


//para poder usarlas en el main
extern t_queue* colaNew;
extern t_list* listaReady;
extern t_list* listaExe;
extern t_list* listaBlock;
extern t_list* listaExit;
extern t_list* tabla_ArchivosAbiertosGlobal;
extern t_list* lista_total_procesos;
extern pthread_mutex_t mutexNew;
extern pthread_mutex_t mutexReady;
extern pthread_mutex_t mutexBlock;
extern pthread_mutex_t mutexExe;
extern pthread_mutex_t mutexExit;
extern pthread_mutex_t multiprocesamiento;
extern pthread_mutex_t mutexHiloTruncate;
extern pthread_mutex_t mutexHiloRead;
extern pthread_mutex_t mutexHiloWrite;

extern sem_t contadorNew;
extern sem_t contadorReady;
extern sem_t contadorExe;
extern sem_t contadorBlock;
//sem_t contadorProcesosEnMemoria;
extern sem_t multiprogramacion;
extern sem_t semFWrite;
extern sem_t semFRead;

extern sem_t largoPlazo;
#endif
