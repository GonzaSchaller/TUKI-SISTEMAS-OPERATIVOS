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
bool seg_con_id_igual(void* segmento);

//TABAL DE SEGMENTOS OCUPADOS
void insertar_segmento_entso(segmento_t * );
segmento_t* encontrar_base_tso(uint32_t id);
void remover_segmento_entso(uint32_t id);

//TABLA DE SEGMENTOS LIBRES
void insertar_segmento_entsl(segmento_t* segmento);
void remove_segmento_tsl(uint32_t id);
bool bypid(void* segmento);

//MEMORIA PRINCIPAL
void memsetear_mp(uint32_t base,uint32_t tamanio,uint32_t contenido);
void* get_contenido(uint32_t offset, uint32_t tamanio);
void set_contenido(void*contenido, uint32_t offset,uint32_t size);

//RELLENO
t_list * create_list_seg_by_pid(uint32_t pid);
t_list* remover_xID(t_list* tabla);
uint32_t find_id(t_list* tsegmentos_pid,uint32_t id);
uint32_t size_tso();
segmento_t* find_en_tsl_rango(uint32_t numero);
segmento_t* get_en_lso(uint32_t pos);
void actualizar_memoria_principal(uint32_t inicio,uint32_t destino, uint32_t tamanio);
void sort_lista_por_ids(t_list*lista);

#endif
