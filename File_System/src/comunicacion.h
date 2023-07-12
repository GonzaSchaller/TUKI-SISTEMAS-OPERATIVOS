#ifndef COMUNICACION_H
#define COMUNICACION_H

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
#include <send_rcvs.h>
#include "iniciar.h"
#include "manejoFS.h"


int generar_conexion_con_memoria();
void conexion_kernel();

#endif
