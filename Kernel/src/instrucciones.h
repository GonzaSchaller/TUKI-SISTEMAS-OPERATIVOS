#ifndef INSTRUCCIONES_H
#define INSTRUCCIONES_H

#include <stdlib.h>
#include <inttypes.h>
#include <commons/collections/list.h>
#include <estructuras_compartidas.h>
#include "configuraciones.h"
#include <send_rcvs.h>

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
    registros_cpu registros;
    t_list* TSegmento;
    uint32_t estimacion_prox_rafaga; //S del HRRN
    float horaDeIngresoAReady; // Seria el timestamp en que el proceso llego a ready cambiar nombre
    t_list* tabla_archivosAbiertos ;// lista de fcb_t
    estado state ; //capaz ponerlo uint32

    float rafaga_anterior_real; // 0 al principio, despues es el tiempo real que se ejecuto. Seria el R(n)
    float estimacion_rafaga_anterior; // estimacion inicial
    float horaDeIngresoAExe; // timestamp cuando llega a execute para calcular la rafaga anterior
	float horaDeSalidaDeExe;
	float hrrn;
	uint32_t tiempo_bloqueo;
}pcb_t;
void enviar_pcb_cpu(int , pcb_t*);
void cargar_instruccion1(int, char* , uint32_t, uint32_t ,uint32_t ,t_list* );
void cargar_instruccion2(int, char* , uint32_t, char* ,uint32_t ,t_list* );
void cargar_instruccion3(int, char* , char*, uint32_t ,uint32_t ,t_list* );
void send_instrucciones_kernel_a_cpu(int,pcb_t*);
void inicializarPCB(int, t_list*, pcb_t *);

#endif
