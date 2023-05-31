#include "manejoDeMemoria.h"
extern t_list* segmentos_libres;

void crear_segmento(int cliente_fd){

}

void borrar_segmento(int cliente_fd){

}

//el segmento 0 va a ser una direccion
uint32_t meter_en_memoria(int size, int id){
	//hay que validar si entra primero jsjsjs

	log_info("Metiendo segmento de tamanio %s en memoria /n", size);

	segmento_t hueco = (*proximo_hueco)(size);



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









// busca el hueco mas chico donde entre el proceso
segmento_t* proximo_hueco_best_fit(uint32_t tamanio){
	//TODO
	segmento_t* seg;

	return seg;
}
// busca el hueco mas garnde donde entre el proceso
segmento_t* proximo_hueco_worst_fit(uint32_t tamanio){
	//TODO
	segmento_t* seg;
		return seg;
}

//busca el primer hueco disponible desde el comienzo de la memoria donde encaje.
segmento_t* proximo_hueco_first_fit(uint32_t tamanio){

	return list_find_first_by_min_size_seglib(tamanio); // de la lista de huecos libres voy

		return seg;
}


// devuelve la direccion del hueco que es menor.
static void* hueco_menor(void* h1, void* h2) {
	segmento_t* hueco1 = (segmento_t*) hueco1;
		segmento_t* hueco2 = (segmento_t*) hueco2;
		if(hueco1->tamanio < hueco2->tamanio) return hueco1;
		else return hueco2;
}
//devuelve la direccion del hueco que es mayor
static void* hueco_mayor(void * h1,void*h2){
	segmento_t* hueco1 = (segmento_t*) hueco1;
	segmento_t* hueco2 = (segmento_t*) hueco2;
	if(hueco1->tamanio > hueco2->tamanio) return hueco1;
	else return hueco2;
}

//funcion que devuelva el tamanio del segmento de tamanio mayor.
//static int tamanio_hueco_mayor(void)
