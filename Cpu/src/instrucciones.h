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

void recibir_instrucciones(int, t_log*);
void cargar_instruccion_a_lista(int, op_code, t_list*, t_log*);
bool verificacion_recibo_code_correctamente(int, t_log*, op_code);
uint32_t recibir_cant_instrucciones(int, t_log*);

#endif
