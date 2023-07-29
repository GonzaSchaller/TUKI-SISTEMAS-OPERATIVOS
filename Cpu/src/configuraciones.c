#include "configuraciones.h"

t_log* logger;
t_config* config;

char* ip;
char* puerto_cpu;
char* ip_memoria;
char* puerto_memoria;
int retardo_instruccion;
int tam_max_segmento;
int conexion;

void levantar_config(){
    puerto_cpu = config_get_string_value(config, "PUERTO_ESCUCHA");
    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    puerto_memoria = config_get_string_value(config,"PUERTO_MEMORIA");
    retardo_instruccion = config_get_int_value(config,"RETARDO_INSTRUCCION"); //cambie de char a int para usarlo directamente
    tam_max_segmento = config_get_int_value(config,"TAM_MAX_SEGMENTO");
}
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
