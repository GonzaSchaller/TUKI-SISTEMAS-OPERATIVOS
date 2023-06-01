
#ifndef FUNCIONES
#define FUNCIONES

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <commons/log.h>
#include <sockets.h>
#include <estructuras_compartidas.h>
#include <string.h>
#include <commons/collections/list.h>
#include "send_rcvs.h"
#include "conexiones_consola.h"

int obtener_registro_cpu(char*);
void obtener_instrucciones(char*);
void send_instrucciones_a_kernel();


#endif
