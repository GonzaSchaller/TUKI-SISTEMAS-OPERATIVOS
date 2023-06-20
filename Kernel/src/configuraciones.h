#ifndef SRC_CONFIGURACIONES_H_
#define SRC_CONFIGURACIONES_H_
#include "inttypes.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <commons/collections/queue.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <pthread.h>
typedef struct{
	char* nombre;
	int instancia;
	t_queue* colaBloqueados;
	pthread_mutex_t mutexRecurso;
}recurso_sistema;

typedef enum{
	 FIFO,
	 HRRN
}algoritmo_planif;

extern char* ip;
extern char* puerto_escucha;
extern char* ip_cpu;
extern char* puerto_cpu;
extern char* ip_memoria;
extern char* puerto_memoria;
extern char* ip_fileSystem;
extern char* puerto_fileSystem;
extern char* algoritmo_planificacion;

extern uint32_t estimacion_inicial;
extern uint32_t grado_max_multiprogramacion;
extern float hrrn_alfa;
extern t_list* lista_recursos;


void validar_alfa(float);
t_list* obtener_recursos(t_config*, char*, char*);
int obtener_algoritmo_planificacion(char* );
#endif /* SRC_CONFIGURACIONES_H_ */
