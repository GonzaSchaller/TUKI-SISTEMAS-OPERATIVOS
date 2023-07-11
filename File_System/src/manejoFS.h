#ifndef SEND_RCVS_H_
#define SEND_RCVS_H_

#include <stdint.h> // pasarlo a todos los archivos
#include <stdio.h>
#include <stdlib.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>

#include <sockets.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include <sys/mman.h>
#include <commons/bitarray.h>
#include <math.h>
#include "iniciar.h"
#include <stdbool.h>

bool existe_y_abrir(char*nombre_archivo);
bool crear_archivo(char*nombre,uint32_t tamanio);
char* buscar_contenido(uint32_t puntero,uint32_t cb);
bool escribir_contenido(char* contenido,uint32_t punterow);

#endif