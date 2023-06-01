
#ifndef KERNEL_MAINH
#define KERNEL_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <sockets.h>
#include <pthread.h>
#include <send_rcvs.h>

#include "conexiones_kernel.h"
#include "planificador.h"
#include "instrucciones.h"
#include "configuraciones.h"

extern int conexion_cpu;
extern int conexion_fileSystem;
extern int conexion_memoria;

extern t_log* log_kernel;

void iniciar_config(t_config* );
void liberarConexiones(int , int , int );
void terminar_kernel(t_config*);
void generar_conexiones();
int server_escuchar(int);



#endif
