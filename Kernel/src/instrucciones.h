#ifndef INSTRUCCIONES_H
#define INSTRUCCIONES_H

#include <inttypes.h>
#include <send_rcvs.h>
#include <commons/collections/list.h>

typedef struct{
	int id;
	char* nombre; //set
	uint32_t parametro1; //ax
	uint32_t parametro2; // "ASfas"
	uint32_t parametro3;
}instruccion;
//
typedef enum {
    NEW,
    READY,
    EXEC,
    BLOCK,
    FINISH
}estado;

typedef struct{
    uint32_t id;
    uint32_t direccion_Base;
    uint32_t tamanio;
}tabla_Segmentos;

typedef struct{
        int posicion_puntero;
        t_list* archivos_Abiertos;
} tabla_Archivos_Abiertos;

//enum para mandar las instrucciones
typedef enum{
	AX,
	BX,
	CX,
	DX,
	EAX,
	EBX,
	ECX,
	EDX,
	RAX,
	RBX,
	RCX,
	RDX
}Registro;
//set ax "sada"

//Valores de registros que voy a guardar en el pcb
typedef struct{
//	   char AX[4]; // Registro AX de 4 bytes
//	   char BX[4];
//	   char CX[4];
//	   char DX[4];
//	   char EAX[8]; // Registro EAX de 8 bytes
//	   char EBX[8];
//	   char ECX[8];
//	   char EDX[8];
//	   char RAX[16]; // Registro RAX de 16 bytes
//	   char RBX[16];
//	   char RCX[16];
//	   char RDX[16];

}registros_cpu;

//---------------------------------------------HASTA ACA--------------------------
//typedef struct  {
//        Registro registro;
//        char* valor;
//} registroCpu;

typedef enum{
    HRRN,
    FIFO
} t_algoritmo_planificacion;


typedef struct{
    uint32_t PID; // Debe ser unico en tod el sistema
    t_list* instrucciones; // Lista de instrucciones a ejecutar
    uint32_t PC; // Program counter
    registros_cpu registros_cpu;
    tabla_Segmentos TSegmento;
    uint32_t estimacion_prox_rafaga;
    uint32_t tiempo_ready; // Seria el timestamp en que el proceso llego a ready cambiar nombre
    tabla_Archivos_Abiertos tabla_archivos ;// No se si conviene esto o una estructura
    estado state ; //capaz ponerlo uint32

//  float rafagaAnterior;
    float estimacion_actual;
    float estimacion_anterior;
    //float horaDeIngresoAExe;
}pcb_t;

void cargar_instruccion(int, char* , uint32_t, uint32_t ,uint32_t ,t_list* );
void inicializarPCB(int, t_list*, pcb_t *);

#endif
