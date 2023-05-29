#ifndef SRC_ESTRUCTURAS_COMPARTIDAS_H_
#define SRC_ESTRUCTURAS_COMPARTIDAS_H_
#include <commons/collections/queue.h>

typedef struct{
    uint32_t id;
    uint32_t direccion_Base;
    uint32_t tamanio;
}segmento_t;

typedef enum{
   	 EXITOSO,
   	 FALLIDO,
   	 COMPACTAR
} estados_segmentos;

typedef struct{ //A implmentar por FileSystem
	char* nombreArchivo;
	uint32_t puntero_directo;
	uint32_t puntero_indirecto;
	//... algo mas
	t_queue* colaBloqueados;
	pthread_mutex_t mutexArchivo; //para garantizar mutua exclusion //todo ver donde inicializarlo

} fcb_t;

#endif /* SRC_ESTRUCTURAS_COMPARTIDAS_H_ */
