#include "mmu.h"

uint32_t num_segmento(uint32_t dir_logica){
	return floor(dir_logica / tam_max_segmento);
}

uint32_t desplazamiento_segmento(uint32_t dir_logica){
	return dir_logica % tam_max_segmento; //REVISAR
}


uint32_t obtener_dir_fisica(uint32_t dir_logica, t_list* lista_segmentos){
	uint32_t segmento = num_segmento(dir_logica);
	uint32_t offset = desplazamiento_segmento(dir_logica);
	uint32_t dir_Base = list_get(lista_segmentos,segmento);

	return dir_Base + offset;
}
//La tabla de segmentos esta ordenada por id son de tipo segmento_t

