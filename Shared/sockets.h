#ifndef SOCKETS_H
#define SOCKETS_H

#include <stdio.h>
#include <stdlib.h>
#include<commons/log.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

int iniciar_servidor (t_log* , char* , char* );
int esperar_cliente(t_log* ,int);

#endif
