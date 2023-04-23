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


extern t_log* log_kernel;

char* ip;

char* puerto_escucha;

char* ip_cpu;
char* puerto_cpu;
char* ip_memoria;
char* puerto_memoria;
char* ip_fileSystem;
char* puerto_fileSystem;
char* algoritmo_planificacion;
uint32_t estimacion_inicial;

uint32_t grado_max_multiprogramacion;
void iniciar_config(t_config* );
void liberarConexiones(int , int , int );
void terminar_kernel(t_log*,t_config*);
void generar_conexiones(t_log* );




#endif
