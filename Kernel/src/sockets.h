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

int iniciar_servidor (t_log* , char* , char* );
int esperar_cliente(t_log* ,int);
int crear_conexicon(t_log* , char* , char* );

#endif
