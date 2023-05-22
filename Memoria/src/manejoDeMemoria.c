#include "manejoDeMemoria.h"


void crear_segmento(int cliente_fd){

}

void borrar_segmento(int cliente_fd){

}



segmento_t* new_segmento(uint32_t id, uint32_t direccion_base,uint32_t tamanio){
	segmento_t* seg = malloc(sizeof(segmento_t));
	seg->direccion_Base = direccion_base;
	seg->tamanio = tamanio;
	seg->id = id;
	return seg;
}
/*
void asesinar_segmentos_libres(){//mutex TODO
	list_destroy_and_destroy_elements(segmentos_libres,(void*)free);
}
*/
segmento_t* proximo_hueco_best_fit(uint32_t){
	//TODO
	segmento_t* seg;
	return seg;
}

segmento_t* proximo_hueco_worst_fit(uint32_t){
	//TODO
	segmento_t* seg;
		return seg;
}
segmento_t* proximo_hueco_first_fit(uint32_t){
	//TODO
	segmento_t* seg;
		return seg;
}
