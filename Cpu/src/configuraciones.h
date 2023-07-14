#ifndef SRC_CONFIGURACIONES_H
#define SRC_CONFIGURACIONES_H
#include "inttypes.h"
#include <stdlib.h>
#include <stdio.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>

extern char* ip;
extern int retardo_instruccion;
extern char* ip_memoria;
extern char* puerto_memoria;
extern char* puerto_cpu;
extern int tam_max_segmento;

void levantar_config();

#endif
