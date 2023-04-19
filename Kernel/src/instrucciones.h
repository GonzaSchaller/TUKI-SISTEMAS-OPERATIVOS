#ifndef INSTRUCCIONES_H
#define INSTRUCCIONES_H

#include <inttypes.h>
#include <send_rcvs.h>
#include <commons/collections/list.h>


//Codigo de operacions instrucciones


typedef struct{
	int id;
	char* nombre;
	uint32_t parametro1;
	uint32_t parametro2;
	uint32_t parametro3;
}instruccion;
typedef enum {
    NEW,
    READY,
    EXEC,
    BLOCK,
    FINISH
}estado;

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

typedef enum{
    HRRN,
    FIFO
}t_algoritmo_planificacion;


typedef struct{
    uint32_t PID; // Debe ser unico en tod el sistema
    t_list* instrucciones; // Lista de instrucciones a ejecutar
    uint32_t PC; // Program counter
    registroCpu regCpu;
    tablaDeSegmentos TSegmento;
    //uint32_t indice_tabla_segmentos;
    uint32_t estimacionRafaga;
    uint32_t tiempReady; // Seria el timestamp en que el proceso llego a ready cambiar nombre
    tablaArchivosAbiertos tablaArchivos ;// No se si conviene esto o una estructura
    estado state ;

//  float rafagaAnterior;
    float estimacionActual;
    float estimacionAnterior;
    //float horaDeIngresoAExe;
}pcb_t;



void cargar_instruccion(int, char* , uint32_t, uint32_t ,uint32_t ,t_list* );


#endif
