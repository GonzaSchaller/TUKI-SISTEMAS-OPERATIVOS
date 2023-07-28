#ifndef COMUNICACION_H
#define COMUNICACION_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>

#include <sockets.h>
#include <sys/socket.h>
#include <pthread.h>

#include <signal.h>
#include <unistd.h>
#include <netdb.h>

#include <send_rcvs.h>
#include "iniciar.h"
#include "manejoFS.h"


void generar_conexion_con_memoria();
void conexion_kernel();
void procesar_peticiones(int cliente_socket);
void conexion_kernel(t_log* log_memoria,char* server_name, int server_socket);
void aumentar_tamanio_archivo(uint32_t, uint32_t, fcb_t*, t_config*);
void reducir_tamanio_archivo(uint32_t, uint32_t, fcb_t*, t_config*);
#endif
