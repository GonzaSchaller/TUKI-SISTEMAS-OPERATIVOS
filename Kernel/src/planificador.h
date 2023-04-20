#ifndef SRC_PLANIFICADOR_H_
#define SRC_PLANIFICADOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <send_rcvs.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include "instrucciones.h"


typedef struct{
	int socket;
	t_log* log;
	char* server_name;
}args_atender_cliente;

void atender_cliente(void*);

#endif
