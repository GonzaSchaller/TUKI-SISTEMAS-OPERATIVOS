#ifndef INICIAR_H
#define INICIAR_H

#include <stdint.h>
#include <stdio.h>//
#include <stdlib.h>//
#include <stdbool.h>//
#include <pthread.h>//
#include <commons/log.h> //
#include <commons/string.h>//
#include <commons/config.h>//
#include <commons/collections/list.h>//
#include <commons/collections/queue.h>//
#include <readline/readline.h>
#include <assert.h>//
#include <sockets.h>//
#include<signal.h>//
#include<unistd.h>//
#include<sys/socket.h>//
#include<netdb.h>//
#include<string.h>//
#include <estructuras_compartidas.h>



typedef struct {
    char* ALGORITMO_ASIGNACION;
    uint32_t TAMANIO_MEMORIA;//
    uint32_t TAMANIO_SEGMENTO;//
    char* PUERTO_ESCUCHA;//
    uint32_t RETARDO_MEMORIA;//
    uint32_t RETARDO_COMPACTACION;//
    uint32_t IP_MEMORIA;//
    uint32_t CANT_SEGMENTOS;//

} t_config_memoria;


static t_config_memoria* inicializar_cfg() {
    t_config_memoria* cfg = malloc(sizeof(t_config_memoria));
    cfg->ALGORITMO_ASIGNACION=NULL;
    return cfg;
}

uint8_t init();
uint8_t cargar_configuracion(char*path);
uint8_t cargar_memoria();
void terminar_memoria();

#endif
