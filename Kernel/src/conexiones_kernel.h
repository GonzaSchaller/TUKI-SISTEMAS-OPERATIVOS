#ifndef SRC_CONEXIONES_H_
#define SRC_CONEXIONES_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <commons/log.h>
#include <sockets.h>
#include "kernel.h"

typedef struct{
	int socket;
	t_log* log;
	char* server_name;
}args_atender_cliente;

void procesar_conexion_consola(void*);
void procesar_conexion_cpu(void*);
void procesar_conexion_fileSystem(void*);
void procesar_conexion_memoria(void*);
void escuchar_clientes(t_log*, int );

#endif
