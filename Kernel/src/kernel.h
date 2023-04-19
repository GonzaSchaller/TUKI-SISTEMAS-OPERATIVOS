#ifndef KERNEL_MAINH
#define KERNEL_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <sockets.h>
#include <pthread.h>
#include <send_rcvs.h>
//#include "planificacion.h"
#include "instrucciones.h"

int contadorProcesos = 0 ;
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
void atender_cliente(void*);
void escuchar_clientes(t_log*, int);
void procesar_conexion_cpu(void*);
void procesar_conexion_fileSystem(void*);
void procesar_conexion_memoria(void*);
void generar_conexiones(t_log* );



#endif
