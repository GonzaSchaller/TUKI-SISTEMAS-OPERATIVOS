
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


void iniciar_config(t_config* );
void liberarConexiones(int , int , int );
void terminar_kernel(t_log*,t_config*);
void generar_conexiones(t_log* );




#endif
