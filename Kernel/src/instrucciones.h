#ifndef INSTRUCCIONES_H
#define INSTRUCCIONES_H

#include <stdlib.h>
#include <inttypes.h>
#include <commons/collections/list.h>
#include <estructuras_compartidas.h>
#include "configuraciones.h"
#include <send_rcvs.h>

typedef struct {
	char* nombreArchivo;
	uint32_t tamanio;
	t_list* colaBloqueados;
	pthread_mutex_t mutexArchivo;

}fcb_kernel;
typedef struct{
	char* nombreArchivo;
	uint32_t puntero;
}fcb_por_proceso;

void enviar_pcb_cpu(int , pcb_t*);
void cargar_instruccion1(int, char* , uint32_t, uint32_t ,uint32_t ,t_list* );
void cargar_instruccion2(int, char* , uint32_t, char* ,uint32_t ,t_list* );
void cargar_instruccion3(int, char* , char*, uint32_t ,uint32_t ,t_list* );
void send_instrucciones_kernel_a_cpu(int,pcb_t*);
void inicializarPCB(int, t_list*, pcb_t *, int);
void asignar_recurso(char* ,t_list* );
void aumentar_instancias_recurso(char*,t_list* );

extern recurso_sistema* encontrar_recurso(t_list*,char*);

#endif
