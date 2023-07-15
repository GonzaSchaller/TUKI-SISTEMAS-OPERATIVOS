#include "mmu.h"

//extern t_log* logger;

uint32_t num_segmento(uint32_t dir_logica){
    return floor(dir_logica / tam_max_segmento);
}

uint32_t desplazamiento_segmento(uint32_t dir_logica){
    return dir_logica % tam_max_segmento;
}

void obtener_dir_fisica(uint32_t dir_logica, t_list* lista_segmentos, uint32_t* segmento, uint32_t* offset, uint32_t* dir_Base, uint32_t* tamanio, uint32_t* dir_Fisica) {
    *segmento = num_segmento(dir_logica);
    *offset = desplazamiento_segmento(dir_logica);

    segmento_t* info_segmento = list_get(lista_segmentos, *segmento);
    *dir_Base = info_segmento->direccion_Base;
    *tamanio = info_segmento->tamanio;

    if (*dir_Base + *offset < *dir_Base + *tamanio) {
        *dir_Fisica = *dir_Base + *offset;
    } else {
        *dir_Fisica = -1;
    }
}

