#include "manejoDeMemoria.h"
//log
extern t_log*log_memoria;

//estructuras auxiliares
extern t_list* segmentos_libres;
//extern t_list* segmentos_ocupados;
extern int memoria_disponible;
extern int tam_hueco_mas_grande;
//extern void* memoria_principal;

//funcion Algortimo de asignacion
extern segmento_t* (*proximo_hueco)(uint32_t);

//static
static uint32_t tamanioStc = 0;
#define POZO 9999

//semaforos
//extern pthread_mutex_t mutex_segmentos_usados;
extern pthread_mutex_t mutex_segmentos_libres;
//extern pthread_mutex_t mutex_memoria_ocupada;

bool entra_en_memoria(uint32_t size){
	return memoria_disponible >= size;
}
bool entra_en_hueco_mas_grande(uint32_t size){
	return tam_hueco_mas_grande >= size;
}

static bool segmento_entra(void* segmento){
	segmento_t* seg = (segmento_t*) segmento;
	return seg->tamanio >= tamanioStc;
}


static void* hueco_menor(void* h1, void* h2) {
	segmento_t* hueco1 = (segmento_t*) h1;
	segmento_t* hueco2 = (segmento_t*) h2;
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

segmento_t* new_segmento(uint32_t id, uint32_t direccion_base,uint32_t tamanio,uint32_t pid){
	segmento_t* seg = malloc(sizeof(segmento_t));
	seg->direccion_Base = direccion_base;
	seg->tamanio = tamanio;
	seg->id = id;
	seg->pid = pid;
	return seg;
}

///// no relleno


bool actualizar_segmentos_libres (segmento_t* seg, uint32_t size){
	if(seg->tamanio > size){
		seg->tamanio-= size;
		seg->direccion_Base +=size;
	} else { //eliminar por base?
		remove_segmento_tsl(seg->direccion_Base);
}

if(seg->tamanio > tam_hueco_mas_grande) tam_hueco_mas_grande = seg->tamanio;
	return 0;
}


segmento_t* crear_segmento(uint32_t id,uint32_t size,uint32_t pid){

	segmento_t * seg = (*proximo_hueco)(size);
	if(seg==NULL){
		log_error(log_memoria,"no pude agarrar el hueco");
	}
	uint32_t base = seg ->direccion_Base;

	log_info(log_memoria,"“PID: %d - Crear Segmento: %d - TAMAÑO: %d",pid,id,size);

	segmento_t* segmento_libre = new_segmento(id,base,size,pid);

	insertar_segmento_entso(segmento_libre);

	//actualizar los huecos libres y el tamanio del seg maximo.
	if(actualizar_segmentos_libres(seg,size)){
		log_info(log_memoria,"si se pudo actualizar");
	}

	memoria_disponible -= size;

	return segmento_libre;
}


bool borrar_segmento(uint32_t base,uint32_t pid){
	segmento_t* seg = encontrar_base_tso(base);
	if(seg == NULL) return false;
	log_info(log_memoria,"PID: %d - Eliminar Segmento: %d  - Base: %d - Tamanio %d \n",pid,seg->id,seg->direccion_Base,seg->tamanio);

	segmento_t* new_hueco_libre = new_segmento(POZO,seg->direccion_Base,seg->tamanio,POZO);

	insertar_segmento_entsl(new_hueco_libre);

	memsetear_mp(seg->direccion_Base,seg->tamanio,0);


	remover_segmento_entso(seg->direccion_Base);


	pthread_mutex_lock(&mutex_segmentos_libres);
	uint32_t tamanio_si_unifico = unificar_huecos_tsl();
	pthread_mutex_unlock(&mutex_segmentos_libres);


	if(tamanio_si_unifico != 0){
		if(tamanio_si_unifico > tam_hueco_mas_grande) tam_hueco_mas_grande = tamanio_si_unifico;
	}
	else{
		if(new_hueco_libre->tamanio > tam_hueco_mas_grande) tam_hueco_mas_grande = new_hueco_libre->tamanio;
	}
	memoria_disponible += seg->tamanio;

	return 0;
}



t_list* actualizar_tabla_kernel(t_list* tabla){
		t_list* ts_kernel;
		ts_kernel = remover_xID(tabla);
		return ts_kernel;
}

uint32_t unificar_huecos_tsl() {
	uint32_t size_total = 0;
    int size = list_size(segmentos_libres); // saco el lenght a la lista de segmentos libres.
    for (int i=0; i<size; ++i) { //la recorro.
        if (i==size-1) break;
        segmento_t* hueco = list_get(segmentos_libres, i);
        segmento_t* hueco_next = list_get(segmentos_libres, i+1);
        if (hueco->direccion_Base + hueco->tamanio == hueco_next->direccion_Base) {
        size_total = hueco->tamanio + hueco_next->tamanio;
            hueco->tamanio += hueco_next->tamanio;
            list_remove_and_destroy_element(segmentos_libres, i+1, (void*) free);
            i--;
            --size;
        }
    }
    return size_total;
}


segmento_t* proximo_hueco_best_fit(uint32_t tamanio){
	tamanioStc= tamanio;
	pthread_mutex_lock(&mutex_segmentos_libres);
	t_list* huecos_disponibles = list_filter(segmentos_libres,&segmento_entra);
	pthread_mutex_unlock(&mutex_segmentos_libres);
	segmento_t *seg = (segmento_t*)list_get_minimum(huecos_disponibles,(void*)&hueco_menor);
	return seg;
}

segmento_t* proximo_hueco_worst_fit(uint32_t tamanio){
	tamanioStc= tamanio;
	pthread_mutex_lock(&mutex_segmentos_libres);
	t_list* huecos_disponibles = list_filter(segmentos_libres,&segmento_entra);
	pthread_mutex_unlock(&mutex_segmentos_libres);
	segmento_t *seg = (segmento_t*)list_get_maximum(huecos_disponibles,(void*)&hueco_mayor);
	return seg;
}


segmento_t* proximo_hueco_first_fit(uint32_t tamanio){
	tamanioStc = tamanio;
	pthread_mutex_lock(&mutex_segmentos_libres);
	segmento_t *seg = (segmento_t*) list_find(segmentos_libres,&segmento_entra); // se castea porque me devuelve un void *
	pthread_mutex_unlock(&mutex_segmentos_libres);
	return seg;
}

bool compactar(uint32_t iteracion){
	segmento_t* segmento = get_en_lso(iteracion);
	if(segmento->direccion_Base == 0) return true;

	segmento_t*hueco = find_en_tsl_rango(segmento->direccion_Base-1);

	uint32_t base_actual = segmento->direccion_Base;
	uint32_t base_nueva = hueco->direccion_Base;

	segmento->direccion_Base = base_nueva;
	hueco->direccion_Base = base_actual + segmento->tamanio;

	segmento_t* hueco2 = find_en_tsl_rango(hueco->direccion_Base+hueco->tamanio); // si encuentra un hueco con la misma base que su limite.
	if(hueco2 != NULL){//lo encontre y tengo q unificar porque son dos huecos seguidos.
		hueco->tamanio += hueco2->tamanio;
		remove_segmento_tsl(hueco2->direccion_Base);
	}

	actualizar_memoria_principal(base_actual,base_nueva,segmento->tamanio);
	return true;
}


uint32_t compactar_memoria(){
	uint32_t size = size_tso();
	for(int i=0;i<size;i++){
		if(!compactar(i)){
			return 1;
		}
	}
	return 0;
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











