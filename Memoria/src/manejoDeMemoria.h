#ifndef MANEJO_DE_MEMORIA_H
#define MANEJO_DE_MEMORIA_H


#include <ctype.h>//
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

#include "iniciar.h"
#include "comunicacion.h"
#include <estructuras_compartidas.h>
#include "sem.h"


segmento_t* proximo_hueco_first_fit(uint32_t);
segmento_t* proximo_hueco_worst_fit(uint32_t);
segmento_t* proximo_hueco_best_fit(uint32_t);
bool entra_en_memoria(uint32_t size);
bool entra_en_hueco_mas_grande(uint32_t size);

void asesinar_segmentos_libres();
segmento_t* new_segmento(uint32_t id, uint32_t direccion_base,uint32_t tamanio);
bool borrar_segmento(uint32_t id);
segmento_t* crear_segmento(uint32_t id,uint32_t size);
uint32_t meter_en_memoria(segmento_t* segmento_a_ubicar);
uint32_t unificar_huecos_tsl();



#endif
