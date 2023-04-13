#ifndef MEMORIA_H
#define MEMORIA_H


#include <stdint.h> // pasarlo a todos los archivos
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>

#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include "../../home/utnso/tp-2023-1c-Kernel-Masters/Shared/sockets.h"

t_log* iniciar_logger(void);
t_config* iniciar_config(void);

void leer_consola(t_log*);
void paquete(int);
void terminar_programa(int, t_log*, t_config*);

int conexion_memoria;
char* ip_memoria = "127.0.0.1";
char* puerto_memoria;

t_config* config_memoria;
t_log* logger_memoria;



#endif
