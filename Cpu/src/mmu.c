#include "mmu.h"

uint32_t num_segmento(uint32_t dir_logica){
	return floor(dir_logica / tam_max_segmento);
}

uint32_t desplazamiento_segmento(uint32_t dir_logica){
	return dir_logica % tam_max_segmento;
}


uint32_t obtener_dir_fisica(uint32_t dir_logica, t_list* lista_segmentos){
	uint32_t segmento = num_segmento(dir_logica);
	uint32_t offset = desplazamiento_segmento(dir_logica);
	segmento_t info_segmento = list_get(lista_segmentos,segmento);
	uint32_t dir_Base = info_segmento.direccion_Base;
	uint32_t tamanio = info_segmento.tamanio;

	if(dir_Base + offset < dir_Base + tamanio)
		return dir_Base + offset;

	//REVISAR !!!!
	//SEGMENTATION FAULT
	return -1;
}
//La tabla de segmentos esta ordenada por id son de tipo segmento_t

