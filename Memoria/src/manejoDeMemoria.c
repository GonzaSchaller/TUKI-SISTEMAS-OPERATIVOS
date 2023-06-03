#include "manejoDeMemoria.h"
//log
extern t_log*log_memoria;

//estructuras auxiliares
extern t_list* segmentos_libres;
extern t_list* segmentos_usados;
extern int memoria_disponible;
extern int tam_hueco_mas_grande;
extern void* memoria_principal;

//funcion Algortimo de asignacion
extern segmento_t* (*proximo_hueco)(uint32_t);

//static
static uint32_t tamanioStc = 0;
static uint32_t magic = 0;


//semaforos
extern pthread_mutex_t mutex_segmentos_usados;
extern pthread_mutex_t mutex_segmentos_libres;
extern pthread_mutex_t mutex_memoria_ocupada;

bool entra_en_memoria(int size){
	return memoria_disponible>=size;
}
bool entra_en_hueco_mas_grande(int size){
	return tam_hueco_mas_grande >= size;
}

static bool segmento_entra(void* segmento){
	segmento_t* seg = (segmento_t*) segmento;
	return seg->tamanio >= tamanioStc;
}
static bool seg_con_id(void* segmento){
	segmento_t* seg = (segmento_t*) segmento;
	return seg->id == magic;
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

segmento_t* new_segmento(uint32_t id, uint32_t direccion_base,uint32_t tamanio){
	segmento_t* seg = malloc(sizeof(segmento_t));
	seg->direccion_Base = direccion_base;
	seg->tamanio = tamanio;
	seg->id = id;
	return seg;
}

///// no relleno


bool actualizar_segmentos_libres (segmento_t* seg, uint32_t size){
	if(seg->tamanio > size){
		seg->tamanio-= size;
		seg->direccion_Base +=size;
	} else { //elimino el segmento quue tenga el id del segmento.
			pthread_mutex_lock(&mutex_segmentos_libres);
			magic = seg->id;
			list_remove_by_condition(segmentos_libres,(void*) &seg_con_id);
			pthread_mutex_unlock(&mutex_segmentos_libres);
		}

	if(seg->tamanio > tam_hueco_mas_grande) tam_hueco_mas_grande = seg->tamanio;
	return 0;
}


segmento_t* crear_segmento(uint32_t id,uint32_t size){

	segmento_t * seg = (*proximo_hueco)(size);
	if(seg==NULL){
		log_error(log_memoria,"no pude agarrar el hueco");
	}
	uint32_t base = seg ->direccion_Base;
	log_info("PID: %d - Crear segmento: &d, = Base: - Tamanio:  %d\n",id,base,size);

	segmento_t* segmento_libre = new_segmento(id,base,size);

	pthread_mutex_lock(&mutex_segmentos_usados);
	list_add(segmentos_usados,segmento_libre);
	pthread_mutex_unlock(&mutex_segmentos_usados);

	//actualizar los huecos libres y el tamanio del seg maximo.
	if(actualizar_segmentos_libres(seg,size)){
		log_info(log_memoria,"si se pudo actualizar");
	}

	memoria_disponible -= size;

	return segmento_libre;
}


bool borrar_segmento(uint32_t id){


	//tengo que buscar el segmeto en mi lista de segmentos usados,
	//borrar lo que habia en ese espacio en memoria?
	//actualizar mi lista de huecos libres, si tengo dos huecoos contiguos los tengo que compactar.
	magic = id;

	pthread_mutex_lock(&mutex_segmentos_usados);
	segmento_t* seg = (segmento_t*) list_find(segmentos_usados,&seg_con_id);
	pthread_mutex_unlock(&mutex_segmentos_usados);
	if(seg == NULL) return false;
	log_info(log_memoria,"PID: <PID> Eliminar Segmento: %d  - Base: %d - Tamanio %d /n",seg->id,seg->direccion_Base,seg->tamanio);

	segmento_t* new_hueco_libre = new_segmento(NULL,seg->direccion_Base,seg->tamanio);
	//meto el segmento nuevo en segmetos libres
	pthread_mutex_lock(&mutex_segmentos_libres);
	list_add(segmentos_libres,new_hueco_libre);
	pthread_mutex_unlock(&mutex_segmentos_libres);
	//poner en 0 la direccion que apuntaba ese segmento en memoria.
	pthread_mutex_lock(&mutex_memoria_ocupada);
	memset(memoria_principal+seg->direccion_Base,0,seg->tamanio);
	pthread_mutex_unlock(&mutex_memoria_ocupada);
	//elimino el segmento usado de la lista de segmentos usados.
	pthread_mutex_lock(&mutex_segmentos_usados);
	magic = seg->id;
	list_remove_by_condition(segmentos_usados,(void*) &seg_con_id);
	pthread_mutex_unlock(&mutex_segmentos_usados);

	//TODO
	//aca hay que fijarse si el segmento que meti es contiguo o otro, si es asi, tengo que hacerlos uno.

	uint32_t tamanio_si_unifico = unificar_huecos_seglib();

	if(tamanio_si_unifico != 0){
		if(tamanio_si_unifico > tam_hueco_mas_grande) tam_hueco_mas_grande = tamanio_si_unifico;
	}
	else{
		if(new_hueco_libre > tam_hueco_mas_grande) tam_hueco_mas_grande = new_hueco_libre->tamanio;
	}

	memoria_disponible += seg->tamanio;

	return 0;
}

t_list* actualizar_tabla_kernel(t_list* tabla){
	list_remove_by_condition(tabla,(void*) &seg_con_id);
	return tabla;
}

uint32_t unificar_huecos_seglib() {
	uint32_t size_total = 0;
    pthread_mutex_lock(&mutex_segmentos_libres);
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
    pthread_mutex_unlock(&mutex_segmentos_libres);
    return size_total;
}

// busca el hueco mas chico donde entre el proceso
segmento_t* proximo_hueco_best_fit(uint32_t tamanio){
	tamanioStc= tamanio;
	pthread_mutex_lock(&mutex_segmentos_libres);
	t_list* huecos_disponibles = list_filter(segmentos_libres,&segmento_entra);
	pthread_mutex_unlock(&mutex_segmentos_libres);
	segmento_t *seg = (segmento_t*)list_get_minimum(huecos_disponibles,(void*)&hueco_menor);
	return seg;
}
// busca el hueco mas garnde donde entre el proceso
segmento_t* proximo_hueco_worst_fit(uint32_t tamanio){
	tamanioStc= tamanio;
		pthread_mutex_lock(&mutex_segmentos_libres);
		t_list* huecos_disponibles = list_filter(segmentos_libres,&segmento_entra);
		pthread_mutex_unlock(&mutex_segmentos_libres);
		segmento_t *seg = (segmento_t*)list_get_maximum(huecos_disponibles,(void*)&hueco_mayor);
		return seg;
}

//busca el primer hueco disponible desde el comienzo de la memoria donde encaje.
segmento_t* proximo_hueco_first_fit(uint32_t tamanio){
	tamanioStc = tamanio;
	pthread_mutex_lock(&mutex_segmentos_libres);
	segmento_t *seg = (segmento_t*) list_find(segmentos_libres,&segmento_entra); // se castea porque me devuelve un void *
	pthread_mutex_unlock(&mutex_segmentos_libres);
		return seg;
}


bool finalizar_estructuras(t_list* tabla_segmentos){

	uint32_t size = lista_size(tabla_segmentos);



	return 0;
}



