#ifndef INSTRUCCIONES_H
#define INSTRUCCIONES_H

#include <inttypes.h>
#include <send_rcvs.h>
#include <commons/collections/list.h>

//#include "kernel.h"
//Codigo de operacions instrucciones
typedef struct{
	int id;
	char* nombre;
	uint32_t parametro1;
	uint32_t parametro2;
	uint32_t parametro3;
}instruccion;


void cargar_instruccion(int, char* , uint32_t, uint32_t ,uint32_t ,t_list* );

#endif
