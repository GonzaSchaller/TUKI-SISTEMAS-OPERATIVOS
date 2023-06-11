#ifndef SEM_H
#define SEM_H


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>
#include <sockets.h>
#include <pthread.h>
#include <estructuras_compartidas.h>

//SEMAFOROS
void iniciar_mutex();
void finalizar_mutex();

//TABAL DE SEGMENTOS OCUPADOS
void insertar_segmento_entso(segmento_t * );
segmento_t* encontrar_id_tso(uint32_t id);
void remover_segmento_entso(uint32_t id);

//TABLA DE SEGMENTOS LIBRES
void insertar_segmento_entsl(segmento_t* segmento);
void remove_segmento_tsl(uint32_t id);
segmento_t* encontrar_id_tso(uint32_t id);

//MEMORIA PRINCIPAL
void memsetear_mp(uint32_t base,uint32_t tamanio);

//RELLENO
t_list* remover_xID(t_list* tabla);



#endif
