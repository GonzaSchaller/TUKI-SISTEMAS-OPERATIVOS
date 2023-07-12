#ifndef MANEJO_FS_H
#define MANEJO_FS_H

#include <stdint.h> // pasarlo a todos los archivos
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/bitarray.h>

#include <sockets.h>
#include <sys/socket.h>
#include <pthread.h>

#include <signal.h>
#include <unistd.h>
#include <netdb.h>

#include <sys/mman.h>

#include "iniciar.h"
#include <stdbool.h>

bool existe_y_abrir(char*nombre_archivo);
bool crear_archivo(char*nombre,uint32_t tamanio);
char* buscar_contenido(uint32_t puntero,uint32_t cb);
bool escribir_contenido(char* contenido,uint32_t punterow);

#endif
