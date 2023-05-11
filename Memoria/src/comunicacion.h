#ifndef SRC_COMUNICACION_H_
#define SRC_COMUNICACION_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <commons/log.h>
#include <sockets.h>


int server_escuchar(t_log *,char*, int);

#endif
