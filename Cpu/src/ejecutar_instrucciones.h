#ifndef INSTRUCCIONES_H
#define INSTRUCCIONES_H

#include <stdint.h> // pasarlo a todos los archivos
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <readline/readline.h>
#include <sockets.h>

#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include "send_rcvs.h"
#include "estructuras_compartidas.h"
#include "recibo_instrucciones.h"
//#include "configuraciones.h"

typedef struct{
	uint32_t PID;
	uint32_t PC;
	t_list* instrucciones;
	registros_cpu registros;

}pcb_cpu;

void pasar_al_registro(char*, char);

#endif
