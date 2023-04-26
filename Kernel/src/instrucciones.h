#ifndef INSTRUCCIONES_H
#define INSTRUCCIONES_H

#include <stdlib.h>
#include <inttypes.h>
#include <commons/collections/list.h>
#include <estructuras_compartidas.h>

typedef union {//porque puedo recibir int o char*
	uint32_t tipo_int;
	char* tipo_string;
 }tipo_parametro;


typedef struct{
	int id;
	char* nombre; //set
	tipo_parametro parametro1;
	tipo_parametro parametro2;
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
        int posicion_puntero;
        t_list* archivos_Abiertos;
} tabla_archivos_abiertos;

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


typedef struct{
    uint32_t PID; // Debe ser unico en tod el sistema
    t_list* instrucciones; // Lista de instrucciones a ejecutar
    uint32_t PC; // Program counter
    registros_cpu registros_cpu;
    tabla_segmentos TSegmento;
    uint32_t estimacion_prox_rafaga;
    uint32_t tiempo_ready; // Seria el timestamp en que el proceso llego a ready cambiar nombre
    tabla_archivos_abiertos tabla_archivos ;// No se si conviene esto o una estructura
    estado state ; //capaz ponerlo uint32

//  float rafagaAnterior;
    float estimacion_actual;
    float estimacion_anterior;
    //float horaDeIngresoAExe;
}pcb_t;

void cargar_instruccion1(int, char* , uint32_t, uint32_t ,uint32_t ,t_list* );
void cargar_instruccion2(int, char* , uint32_t, char* ,uint32_t ,t_list* );
void cargar_instruccion3(int, char* , char*, uint32_t ,uint32_t ,t_list* );

void inicializarPCB(int, t_list*, pcb_t *);

#endif
