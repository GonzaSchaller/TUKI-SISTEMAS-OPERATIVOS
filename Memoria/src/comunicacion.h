#ifndef SRC_COMUNICACION_H_
#define SRC_COMUNICACION_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <commons/log.h>
#include <sockets.h>
#include <send_rcvs.h>


int server_escuchar(t_log *,char*, int);
int server_escuchar3(t_log *,char*, int);
void tirar_threads(int socket_cliente,char*server_name,int server_socket);
//void procesar_conexionn(void* void_args);
#endif
