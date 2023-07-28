#ifndef MANEJO_FS_H
#define MANEJO_FS_H

#include <stdint.h>
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
#include <estructuras_compartidas.h>

bool existe_y_abrir(char*nombre_archivo);
bool crear_archivo(char*nombre,uint32_t tamanio);
char* buscar_contenido(char*nombre,uint32_t puntero,uint32_t cb);
char* leer_en_bloque(fcb_t* fcb, uint32_t puntero, uint32_t cant_bytes, uint32_t enquebloquestoy);
void escribir_en_bloque(fcb_t* fcb, uint32_t puntero, uint32_t cant_bytes, uint32_t enquebloquestoy, char* contenido);
uint32_t ceil_casero(uint32_t nro1,uint32_t nro2);
t_list* bloque_del_archivo (fcb_t* fcb,uint32_t bloque_estoy,uint32_t cant_bloques_a_leer,uint32_t puntero,t_list** bloques_fs);
char* concat(char*nombre_archivo);
t_list*  add_and_reverse(t_list* lista, int nuevo_elemento) ;
t_list* cant_bloques_puntero_indirecto(int puntero_indirecto, int cuantos_bloques_venia_usando);
void llenar_archivo_nuevo(fcb_t* fcb, t_config*);
t_list * buscar_bloques_libres(int cant_bloques_a_agregar);
uint32_t buscar_bloque_libre();
#endif
