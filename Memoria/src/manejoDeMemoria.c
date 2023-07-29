#include "manejoDeMemoria.h"
extern t_log*log_memoria;
extern t_list* segmentos_libres;
extern t_list* segmentos_ocupados;
extern int memoria_disponible;
extern segmento_t* (*proximo_hueco)(uint32_t);
static uint32_t tamanio_alg_asignacion = 0;
#define POZO 9999

bool entra_en_memoria(uint32_t size){
	return memoria_disponible >= size;
}
bool entra_en_hueco_mas_grande(uint32_t size){
	uint32_t tamanio = encontrarTamanioDelSegmentoConMayorTamanio();
	return tamanio>=size;
}
static bool segmento_entra(void* segmento){
	segmento_t* seg = (segmento_t*) segmento;
	return seg->tamanio >= tamanio_alg_asignacion;
}

static void* hueco_menor(void* h1, void* h2) {
	segmento_t* hueco1 = (segmento_t*) h1;
	segmento_t* hueco2 = (segmento_t*) h2;
	return hueco1->tamanio < hueco2->tamanio ? h1 : h2;
}
//devuelve la direccion del hueco que es mayor
static void* hueco_mayor(void * h1,void*h2){
	segmento_t* hueco1 = (segmento_t*) h1;
	segmento_t* hueco2 = (segmento_t*) h2;
	return hueco1->tamanio > hueco2->tamanio ? h1 : h2;
}

segmento_t* proximo_hueco_best_fit(uint32_t tamanio){
	tamanio_alg_asignacion = tamanio;
	t_list* huecos_disponibles = list_filter(segmentos_libres,&segmento_entra);
	mostrar_tsl_actualizado(huecos_disponibles,6);
	segmento_t *seg = (segmento_t*)list_get_minimum(huecos_disponibles,(void*)&hueco_menor);
	//log_info(log_memoria,"segmetno agarrado con best: BASE %d SIZE %d",seg->direccion_Base,seg->tamanio);
	list_destroy(huecos_disponibles);
	return seg;
}

segmento_t* proximo_hueco_worst_fit(uint32_t tamanio){
	tamanio_alg_asignacion = tamanio;
	t_list* huecos_disponibles = list_filter(segmentos_libres,&segmento_entra);
	mostrar_tsl_actualizado(huecos_disponibles,6);
	segmento_t *seg = (segmento_t*)list_get_maximum(huecos_disponibles,(void*)&hueco_mayor);
	//log_info(log_memoria,"segmetno agarrado con worst: BASE %d SIZE %d",seg->direccion_Base,seg->tamanio);
	return seg;
}

segmento_t* proximo_hueco_first_fit(uint32_t tamanio){
	tamanio_alg_asignacion = tamanio;
	segmento_t *seg = (segmento_t*) list_find(segmentos_libres,&segmento_entra); // se castea porque me devuelve un void *
	//log_info(log_memoria,"segmetno agarrado con first: BASE %d SIZE %d",seg->direccion_Base,seg->tamanio);
	return seg;
}
segmento_t* new_segmento(uint32_t id, uint32_t direccion_base,uint32_t tamanio,uint32_t pid){
	segmento_t* seg = malloc(sizeof(segmento_t));
	seg->direccion_Base = direccion_base;
	seg->tamanio = tamanio;
	seg->id = id;
	seg->pid = pid;
	return seg;
}

bool actualizar_segmentos_libres (segmento_t* seg, uint32_t size){
	if(seg->tamanio > size){
		seg->tamanio-= size;
		seg->direccion_Base +=size;
	} else {
		remove_segmento_tsl(seg->direccion_Base);
	}

	return 0;
}
segmento_t* crear_segmento(uint32_t id,uint32_t size,uint32_t pid){
	log_info(log_memoria,"\n");
	segmento_t * seg_a_segmentar = (*proximo_hueco)(size);
	if(seg_a_segmentar==NULL){
		log_error(log_memoria,"no pude agarrar el hueco");
	}
	uint32_t base = seg_a_segmentar ->direccion_Base;
	log_info(log_memoria,"“PID: %d - Crear Segmento: %d - TAMAÑO: %d - BASE: %d --------------------------------------------",pid,id,size,base);
	segmento_t* nuevo_segmento_ocupado = new_segmento(id,base,size,pid);
	insertar_segmento_entso(nuevo_segmento_ocupado);
	//actualizar los huecos libres y el tamanio del seg maximo.
	if(actualizar_segmentos_libres(seg_a_segmentar,size)){
		log_info(log_memoria,"Se actualizaron los segmentos libres");
	}
	memoria_disponible -= size;
	log_info(log_memoria,"Cantidad de memoria disponible %d \n",memoria_disponible);// todo
	return nuevo_segmento_ocupado;
}

bool borrar_segmento(uint32_t base,uint32_t pid){
	log_info(log_memoria,"\n");
	segmento_t* seg = encontrar_base_tso(base);
	if(seg == NULL) return false;
	log_info(log_memoria,"PID: %d - Eliminar Segmento: %d  - Base: %d - Tamanio %d \n --------------------------------------------",pid,seg->id,seg->direccion_Base,seg->tamanio);
	segmento_t* new_hueco_libre = new_segmento(POZO,seg->direccion_Base,seg->tamanio,POZO);
	insertar_segmento_entsl(new_hueco_libre);
	memsetear_mp(seg->direccion_Base,seg->tamanio,0);
	remover_segmento_entso(seg->direccion_Base);
	unificar();
	mostrar_tsl_actualizado(segmentos_libres,0);
	memoria_disponible += seg->tamanio;
	log_info(log_memoria,"Memoria disponible: %d",memoria_disponible);
	return true;
}

t_list* actualizar_tabla_kernel(t_list* tabla){
		t_list* ts_kernel;
		ts_kernel = remover_xID(tabla);
		return ts_kernel;
}

void unificar() {
    uint32_t size = list_size(segmentos_libres); // saco el lenght a la lista de segmentos libres.
    for (int i=0; i<size; ++i) { //la recorro.
        if (i == size-1) break;
        segmento_t* hueco = list_get(segmentos_libres, i);
        segmento_t* siguiente_hueco = list_get(segmentos_libres, i+1);
        if (hueco->direccion_Base + hueco->tamanio == siguiente_hueco->direccion_Base) {
            hueco->tamanio += siguiente_hueco->tamanio;
            list_remove_and_destroy_element(segmentos_libres, i+1, (void*) free);
            i--;
            --size;
        }
    }
}

bool compactar(uint32_t pos){
	segmento_t* segmento = list_get(segmentos_ocupados,pos);
	if(segmento ==NULL) return false;
	if(segmento->direccion_Base == 0) return true;

	segmento_t*hueco = encontrar_en_tsl_hueco_con_rango(segmento->direccion_Base-1);
	if(hueco ==NULL) return true;

	uint32_t base_actual = segmento->direccion_Base;
	uint32_t base_nueva = hueco->direccion_Base;

	segmento->direccion_Base = base_nueva;
	hueco->direccion_Base = base_nueva + segmento->tamanio;
	ordenar_listalsl_por_base();

	segmento_t* hueco2 = encontrar_en_tsl_hueco_con_rango(hueco->direccion_Base+hueco->tamanio); // si encuentra un hueco con la misma base que su limite.
	if(hueco2 != NULL){//lo encontre y tengo q unificar porque son dos huecos seguidos.
		hueco->tamanio += hueco2->tamanio;
		remove_segmento_tsl(hueco2->direccion_Base);
	}
	actualizar_memoria_principal(base_actual,base_nueva,segmento->tamanio);
	return true;
}

bool compactar_memoria(){
	uint32_t size = list_size(segmentos_ocupados);
	for(int i=0;i<size;i++){
		if(!compactar(i)){
			return false;
		}
	}
	return true;
}
//delegacion innecesaria? quizas
char *leer_contenido(uint32_t direccion, uint32_t tamanio){
	char*contenido = (char*)get_contenido(direccion,tamanio);
	contenido[tamanio] = '\0';  // Agregar un carácter nulo al final
	return contenido;
}

bool escribir_contenido(void*contenido,uint32_t offset,uint32_t size){
	set_contenido(contenido, offset, size);
	return true;
}











