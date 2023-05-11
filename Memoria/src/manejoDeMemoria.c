
#include "manejoDeMemoria.h"


void crear_segmento(int cliente_fd){

}

void borrar_segmento(int cliente_fd){

}



tabla_segmentos new_segmento(uint32_t id, uint32_t direccion_base,uint32_t tamanio){
	tabla_segmentos* seg = malloc(sizeof(tabla_segmentos));
	seg->direccion_Base = direccion_base;
	seg->tamanio = tamanio;
	seg->id = id;
	return seg;
}


void asesinar_segmentos_libres(){//mutex TODO
	list_destroy_and_destroy_elements(segmentos_libres,(void*)free);
}

tabla_segmentos* proximo_hueco_best_fit(uint32_t){
	//TODO
}

tabla_segmentos* proximo_hueco_worst_fit(uint32_t){
	//TODO
}
tabla_segmentos* proximo_hueco_first_fit(uint32_t){
	//TODO
}
