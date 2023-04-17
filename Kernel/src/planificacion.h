#ifndef KERNEL_INCLUDE_PLANIFICACION_H_
#define KERNEL_INCLUDE_PLANIFICACION_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>
#include <commons/collections/queue.h>
#include <semaphore.h>
#include <time.h>

int contadorProcesos = 0 ;


typedef struct{
	uint32_t PID; // Debe ser unico en todo el sistema
	t_list* instrucciones; // Lista de instrucciones a ejecutar
	uint32_t PC; // Program counter
	registroCpu regCpu;
	tablaDeSegmentos TSegmento;
	//uint32_t indice_tabla_segmentos;
	uint32_t estimacionRafaga;
	uint32_t tiempReady; // Seria el timestamp en que el proceso llego a ready cambiar nombre
	tablaArchivosAbiertos tablaArchivos ;// No se si conviene esto o una estructura
	estado state ;

//	float rafagaAnterior;
	float estimacionActual;
	float estimacionAnterior;
	//float horaDeIngresoAExe;
}pcb_t;

typedef enum{
	HRRN,
	FIFO
}t_algoritmo_planificacion;

typedef struct{
	uint32_t ID;
	uint32_t DireccionBase;
	uint32_t tamanio;
}tablaDeSegmentos;


typedef struct{
        int posicion_puntero;
        t_list* archivosAbiertos;
    } tablaArchivosAbiertos;

typedef struct  {
    	char nombre[4]; // nombre del registro (AX, BX, CX, DX, EAX, EBX, ECX, EDX, RAX, RBX, RCX, RDX)
        int tamano;     // tamaño en bytes del registro (4, 8, 16)
       // char* direccion; // dirección de memoria del registro
    }registroCpu;
typedef enum {
	NEW,
	READY,
	EXEC,
	BLOCK,
	EXIT

}estado;



#endif KERNEL_INCLUDE_PLANIFICACION_H_
