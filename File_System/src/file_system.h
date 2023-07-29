#ifndef FILE_SYSTEM_MAIN_H
#define FILE_SYSTEM_MAIN_H

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
#include <pthread.h>
#include "iniciar.h"
#include "comunicacion.h"
#include <commons/bitarray.h>
#include "comunicacion.h"
#include "manejoFS.h"

extern int fd_fs;
extern int fd_memoria;
extern char* ip;

t_log* iniciar_logger(void);
t_config* iniciar_config(void);
void leer_consola(t_log*);
void paquete(int);
void terminar_programa(int, t_log*, t_config*);

#endif
