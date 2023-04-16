#ifndef SOCKETS_H
#define SOCKETS_H


#include <stdint.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

t_log* iniciar_logger(void);
int iniciar_servidor (t_log* , char* , char*, char*);
int esperar_cliente(t_log* ,int);
int crear_conexion(t_log* , char*, char* , char* );
void terminar_programa(int , t_log*, t_config* );
void liberar_conexion(int);

#endif
