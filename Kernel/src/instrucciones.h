#ifndef INSTRUCCIONES_H
#define INSTRUCCIONES_H

#include <stdlib.h>
#include <inttypes.h>
#include <commons/collections/list.h>
#include <estructuras_compartidas.h>
#include "configuraciones.h"
#include <send_rcvs.h>

typedef enum {
    NEW,
    READY,
    EXEC,
    BLOCK,
    FINISH
}estado;// ESTADO PCB

typedef struct{

    t_list* instrucciones; // Lista de instrucciones a ejecutar
    contexto_ejecucion contexto; //tiene registros, tabla segmento, pid y PC


    uint32_t estimacion_prox_rafaga; //S del HRRN
    float horaDeIngresoAReady; // Seria el timestamp en que el proceso llego a ready cambiar nombre
    t_list* tabla_archivosAbiertos ;// lista de fcb_t
    estado state ; //capaz ponerlo uint32

    uint32_t state_anterior;
    float rafaga_anterior_real; // 0 al principio, despues es el tiempo real que se ejecuto. Seria el R(n)
    float estimacion_rafaga_anterior; // estimacion inicial
    float horaDeIngresoAExe; // timestamp cuando llega a execute para calcular la rafaga anterior
	float horaDeSalidaDeExe;
	float hrrn;
	t_list* recursos_asignados;
	uint32_t tiempo_bloqueo;
}pcb_t;
void enviar_pcb_cpu(int , pcb_t*);
void cargar_instruccion1(int, char* , uint32_t, uint32_t ,uint32_t ,t_list* );
void cargar_instruccion2(int, char* , uint32_t, char* ,uint32_t ,t_list* );
void cargar_instruccion3(int, char* , char*, uint32_t ,uint32_t ,t_list* );
void send_instrucciones_kernel_a_cpu(int,pcb_t*);
void inicializarPCB(int, t_list*, pcb_t *);
void asignar_recurso(char* ,t_list* );
void aumentar_instancias_recurso(char*,t_list* );

#endif
