// mutex y semaforos
#include "sem.h"
pthread_mutex_t mutex_segmentos_libres;
pthread_mutex_t mutex_segmentos_usados;
pthread_mutex_t mutex_memoria_ocupada;
pthread_mutex_t mutex_compactacion;
pthread_mutex_t mutex_segm_0;

//ESTRUCTURAS
extern t_list* segmentos_libres;
extern t_list* segmentos_ocupados;
static uint32_t magic = 0;
static uint32_t pid_s;
extern void* memoria_principal;

//los inicio.
void iniciar_mutex() {
    pthread_mutex_init(&mutex_segmentos_libres, NULL);
    pthread_mutex_init(&mutex_segmentos_usados, NULL);
    pthread_mutex_init(&mutex_memoria_ocupada, NULL);
    pthread_mutex_init(&mutex_compactacion, NULL);
    pthread_mutex_init(&mutex_segm_0, NULL);

 //   sem_init(&sem_cant_segmentos, 0, 0);

}

//los destruyo.
void finalizar_mutex() {
    pthread_mutex_destroy(&mutex_segmentos_libres);
    pthread_mutex_destroy(&mutex_segmentos_usados);
    pthread_mutex_destroy(&mutex_memoria_ocupada);
    pthread_mutex_destroy(&mutex_compactacion);
    pthread_mutex_destroy(&mutex_segm_0);


  //  sem_destroy(&sem_cant_segmentos);

}

static bool por_base_menor(void* s1, void* s2) {
    segmento_t* seg1 = (segmento_t*) s1;
    segmento_t* seg2 = (segmento_t*) s2;
    return seg1->direccion_Base < seg2->direccion_Base;
}


//insertar unsegmento en tabla de segmentos usados
void insertar_segmento_entso(segmento_t * segmento){
	pthread_mutex_lock(&mutex_segmentos_usados);
	list_add_sorted(segmentos_ocupados, (void*) segmento, &por_base_menor);
	pthread_mutex_unlock(&mutex_segmentos_usados);
}

//ordenada de menor a mayor.
void insertar_segmento_entsl(segmento_t* segmento){
	pthread_mutex_lock(&mutex_segmentos_libres);
	list_add_sorted(segmentos_libres, (void*) segmento, &por_base_menor);
	pthread_mutex_unlock(&mutex_segmentos_libres);
}

//pone en 0 algun lugar de la memoria pricipal despues de que fue borrado un segmento
void memsetear_mp(uint32_t base,uint32_t tamanio){
	pthread_mutex_lock(&mutex_memoria_ocupada);
	memset(memoria_principal+base,0,tamanio);
	pthread_mutex_unlock(&mutex_memoria_ocupada);
}


bool seg_con_id_igual(void* segmento){
	segmento_t* seg = (segmento_t*) segmento;
	return seg->id == magic;
}

bool seg_con_base_igual(void* segmento){
	segmento_t* seg = (segmento_t*) segmento;
	return seg->id == magic;
}

segmento_t* encontrar_base_tso(uint32_t id){
	magic = id;
	pthread_mutex_lock(&mutex_segmentos_usados);
	segmento_t* seg = (segmento_t*) list_find(segmentos_ocupados,&seg_con_base_igual);
	pthread_mutex_unlock(&mutex_segmentos_usados);
	return seg;
}



void remover_segmento_entso(uint32_t id){
	magic = id;
	pthread_mutex_lock(&mutex_segmentos_usados);
	list_remove_by_condition(segmentos_ocupados,(void*) &seg_con_id_igual);
	pthread_mutex_unlock(&mutex_segmentos_usados);
}


//just para el kernel.
t_list* remover_xID(t_list* tabla){
	list_remove_by_condition(tabla,(void*) &seg_con_id_igual);
	return tabla;
}

void remove_segmento_tsl(uint32_t id){
	magic = id;
	pthread_mutex_lock(&mutex_segmentos_libres);
	list_remove_by_condition(segmentos_libres,(void*) &seg_con_id_igual);
	pthread_mutex_unlock(&mutex_segmentos_libres);
}

static bool bypid(void* segmento) {
    segmento_t* seg = (segmento_t*) segmento;
    return seg->pid == pid_s;
}


t_list * create_list_seg_by_pid(uint32_t pid){
	pid_s = pid;
	pthread_mutex_lock(&mutex_segmentos_usados);
	t_list* lista = list_filter(segmentos_ocupados, &bypid);
	pthread_mutex_unlock(&mutex_segmentos_usados);
	return lista;
}

uint32_t find_id(t_list* tsegmentos_pid,uint32_t id){
	magic = id;
	uint32_t base_b = list_find(tsegmentos_pid, &seg_con_id_igual);
	return base_b;
}






