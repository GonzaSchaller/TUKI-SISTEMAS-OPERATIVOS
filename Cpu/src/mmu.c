#include "mmu.h"

uint32_t num_segmento(uint32_t dir_logica, uint32_t tam_segmento){
	return floor(dir_logica / tam_segmento);
}

uint32_t desplazamiento_segmento(uint32_t dir_logica, uint32_t tam_segmento){
	return dir_logica % tam_segmento; //REVISAR
}

//ARREGLAR
uint32_t obtener_dir_fisica(uint32_t dir_logica, uint32_t tam_segmento){
	uint32_t segmento = num_segmento(dir_logica, tam_segmento);
	uint32_t offset = desplazamiento_segmento(dir_logica, tam_segmento);
	return segmento * tam_segmento + offset;
}
//La tabla de segmentos esta ordenada por id son de tipo segmento_t

