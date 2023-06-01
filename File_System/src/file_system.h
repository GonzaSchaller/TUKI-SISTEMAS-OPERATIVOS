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

extern int conexion;
extern char* ip;
extern char* puerto_escucha; //puerto para si mismo (file system)
extern char* puerto_memoria;
extern char* superbloque;
extern char* bitmap;
extern char* bloques;
extern char* fcb;
extern char* retardo_acceso_bloque;

extern t_log* logger;
extern t_config* config;

t_log* iniciar_logger(void);
t_config* iniciar_config(void);
void leer_consola(t_log*);
void paquete(int);
void terminar_programa(int, t_log*, t_config*);

#endif
