#include "mmu.h"

uint32_t num_segmento(uint32_t dir_logica, uint32_t tam_segmento){
	return floor(dir_logica / tam_segmento);
}

uint32_t desplazamiento_segmento(uint32_t dir_logica, uint32_t tam_segmento){
	return dir_logica % tam_segmento; //REVISAR
}
