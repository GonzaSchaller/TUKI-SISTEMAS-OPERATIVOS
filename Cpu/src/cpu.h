#ifndef CPU_H
#define CPU_H

#include <stdint.h> // pasarlo a todos los archivos
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>
#include <sockets.h>

#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include "configuraciones.h"
#include <pthread.h>
#include "send_rcvs.h"
#include "recibo_instrucciones.h"
extern t_log* logger;
extern t_config* config;

extern int socket_cliente_kernel;
extern int conexion_memoria;
/*
extern char* ip;
extern char* puerto_cpu;
extern char* ip_memoria;
extern char* puerto_memoria;
extern char* retardo_instruccion;
extern char* tam_max_segmento;
*/

t_log* iniciar_logger(void);
t_config* iniciar_config(void);
void levantar_config();
void leer_consola(t_log*);
void paquete(int);
void terminar_programa(int, t_log*, t_config*);
void recibir_instrucciones();
void procesar_instrucciones(int, t_log*);
void recibir_cant_instrucciones(int, t_log*, uint32_t* cantidad_instrucciones);
#endif
