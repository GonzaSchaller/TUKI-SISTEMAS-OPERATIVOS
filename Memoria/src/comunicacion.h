#ifndef SRC_COMUNICACION_H_
#define SRC_COMUNICACION_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <pthread.h>
#include <sockets.h>

#include <commons/log.h>
#include <commons/collections/list.h>//

#include <send_rcvs.h>

#include "funcionesAuxiliares.h"
#include "manejoDeMemoria.h"



int server_escuchar(t_log *,char*, int);
void procesar_conexionn(void* void_args);


#endif
