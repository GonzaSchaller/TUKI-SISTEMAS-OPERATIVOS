#ifndef INICIAR_H
#define INICIAR_H

#include <stdint.h>
#include <stdio.h>//
#include <stdlib.h>//
#include <stdbool.h>//
#include <string.h>//


#include <commons/log.h> //
#include <commons/string.h>//
#include <commons/config.h>//
#include <commons/collections/list.h>//

#include <pthread.h>//
#include <sockets.h>//
#include <sys/socket.h>//



#include <estructuras_compartidas.h>

#include "funcionesAuxiliares.h"
#include "manejoDeMemoria.h"

extern t_list* lista_de_pids;
extern t_list* segmentos_libres;
extern t_list* segmentos_ocupados;
extern segmento_t* segmento_0;
extern void* memoria_principal;
extern int memoria_disponible;
extern segmento_t* (*proximo_hueco) (uint32_t);

typedef struct {
    char* ALGORITMO_ASIGNACION;
    uint32_t TAMANIO_MEMORIA;//
    uint32_t TAMANIO_SEGMENTO_0;//
    char* PUERTO_ESCUCHA;//
    uint32_t RETARDO_MEMORIA;//
    uint32_t RETARDO_COMPACTACION;//
    uint32_t IP_MEMORIA;//
    uint32_t CANT_SEGMENTOS;//

} t_config_memoria;


void cargar_configuracion(char*path);
void cargar_memoria();
void terminar_memoria();

#endif
