#include "funcionesAuxiliares.h"

extern t_log*log_memoria;
pthread_mutex_t mutex_memoria_ocupada;
pthread_mutex_t mutex_read_write;
extern t_list* lista_de_pids;
extern t_list* segmentos_libres;
extern t_list* segmentos_ocupados;
static uint32_t base_a_buscar = 0;
static uint32_t id_a_buscar = 0;
static uint32_t pid_a_buscar=0;
static uint32_t rango = 0;
extern void* memoria_principal;

void iniciar_mutex() {
    pthread_mutex_init(&mutex_memoria_ocupada, NULL);
    pthread_mutex_init(&mutex_read_write, NULL);
}
void finalizar_mutex() {
    pthread_mutex_destroy(&mutex_memoria_ocupada);
    pthread_mutex_destroy(&mutex_read_write);
}
uint32_t encontrarTamanioDelSegmentoConMayorTamanio() {
	uint32_t tamanio_a_buscar=0;
    uint32_t tamanio_lista = list_size(segmentos_libres);
    for(int i=0;i<tamanio_lista;i++){
    	segmento_t *segmento=(segmento_t*)list_get(segmentos_libres,i);
    	if(segmento->tamanio > tamanio_a_buscar){
    		tamanio_a_buscar = segmento->tamanio;
    	}
    }
    return tamanio_a_buscar;
}
static bool por_base_menor(void* s1, void* s2) {
    segmento_t* seg1 = (segmento_t*) s1;
    segmento_t* seg2 = (segmento_t*) s2;
    return seg1->direccion_Base < seg2->direccion_Base;
}
static bool por_ID_menor(void* s1, void* s2) {
    segmento_t* seg1 = (segmento_t*) s1;
    segmento_t* seg2 = (segmento_t*) s2;
    return seg1->id < seg2->id;
}
void ordenar_lista_por_ids(t_list*lista){
	list_sort(lista, &por_ID_menor);
}
void insertar_segmento_entso(segmento_t * segmento){
	list_add_sorted(segmentos_ocupados, (void*) segmento, &por_base_menor);
}
void insertar_segmento_entsl(segmento_t* segmento){
	list_add_sorted(segmentos_libres, (void*) segmento, &por_base_menor);
}
void memsetear_mp(uint32_t base,uint32_t tamanio,uint32_t contenido){
	pthread_mutex_lock(&mutex_memoria_ocupada);
	memset(memoria_principal+base,contenido,tamanio);
	pthread_mutex_unlock(&mutex_memoria_ocupada);
}
bool seg_con_id_igual(void* segmento){
	segmento_t* seg = (segmento_t*) segmento;
	return seg->id == id_a_buscar;
}
bool seg_con_base_igual(void* segmento){
	segmento_t* seg = (segmento_t*) segmento;
	return seg->direccion_Base == base_a_buscar;
}
segmento_t* encontrar_base_tso(uint32_t base){
	base_a_buscar = base;
	segmento_t* seg = (segmento_t*) list_find(segmentos_ocupados,&seg_con_base_igual);
//	log_info(log_memoria,"Segmento encontrado : base %d , id %d , tamanio %d ", seg->direccion_Base,seg->id,seg->tamanio);
	return seg;
}
void remover_segmento_entso(uint32_t id){
	base_a_buscar = id;
	list_remove_by_condition(segmentos_ocupados,(void*) &seg_con_base_igual);
}
t_list* remover_xID(t_list* tabla){
	list_remove_by_condition(tabla,(void*) &seg_con_id_igual);
	return tabla;
}
void remove_segmento_tsl(uint32_t base){
	base_a_buscar = base;
	list_remove_by_condition(segmentos_libres,(void*) &seg_con_base_igual);
}
bool por_pid_igual(void* segmento) {
    segmento_t* seg = (segmento_t*) segmento;
    return seg->pid == pid_a_buscar;
}
t_list* filtrar_lista_por_pid(uint32_t pid){
	pid_a_buscar = pid;
	t_list* lista = list_filter(segmentos_ocupados,&por_pid_igual);
	return lista;
}
t_list * create_list_seg_by_pid(uint32_t pid){
	pid_a_buscar = pid;
	t_list* lista = list_filter(segmentos_ocupados, &por_pid_igual);
	return lista;
}
uint32_t buscar_en_lista_por_id_devolver_base(t_list* tsegmentos_pid,uint32_t id){
	id_a_buscar = id;
	segmento_t*segmento = list_find(tsegmentos_pid, &seg_con_id_igual);
	return segmento->direccion_Base;
}

bool esta_en_rango(void* segmento){
	segmento_t* seg = (segmento_t*) segmento;
	return (rango >= seg->direccion_Base ) && (rango < (seg->direccion_Base + seg->tamanio)); //    base <= rango < base+tamanio(limite)
}

segmento_t* encontrar_en_tsl_hueco_con_rango(uint32_t numero){
	rango = numero;
	segmento_t*hueco = (segmento_t*)list_find(segmentos_libres, &esta_en_rango);
	return hueco;
}
void actualizar_memoria_principal(uint32_t inicio,uint32_t destino, uint32_t tamanio){
    void* data = malloc(tamanio);
    pthread_mutex_lock(&mutex_memoria_ocupada);
    memcpy(data, memoria_principal+inicio, tamanio);
    memset(memoria_principal+inicio, 0, tamanio);
    memcpy(memoria_principal+destino, data, tamanio);
    pthread_mutex_unlock(&mutex_memoria_ocupada);
    free(data);
}
void* get_contenido(uint32_t offset, uint32_t size) {
    void* stream = malloc(size);
    pthread_mutex_lock(&mutex_memoria_ocupada);
    memcpy(stream, memoria_principal+offset, size);
    pthread_mutex_unlock(&mutex_memoria_ocupada);
    return stream;
}
void set_contenido(void* contenido,uint32_t offset, uint32_t size) {
    pthread_mutex_lock(&mutex_memoria_ocupada);
    memcpy(memoria_principal+offset, contenido, size);
    pthread_mutex_unlock(&mutex_memoria_ocupada);
}
void ordenar_listalsl_por_base(){
	list_sort(segmentos_libres,&por_base_menor);
}
bool por_pid_menor_a_mayor(void* pid1,void*pid2) {
	uint32_t p1 = *(uint32_t*) pid1;
	uint32_t p2 = *(uint32_t*)pid2;
    return p1 < p2;
}
void ordenar_lista_pid_por_pid(){
	list_sort(lista_de_pids,&por_pid_menor_a_mayor);
}
bool pid_igual(void*pid){
	uint32_t p = *(uint32_t*) pid;
	return p == pid_a_buscar;
}
void eliminar_pid_lista_pids(uint32_t pid){
	pid_a_buscar = pid;
	list_remove_and_destroy_by_condition(lista_de_pids,&pid_igual,free);
	uint32_t size = list_size(lista_de_pids);
	//log_info(log_memoria,"Size lista pids %d",size);
	//for(int i =0;i<size;i++){
		//uint32_t *pid = list_get(lista_de_pids,i);
		//log_info(log_memoria,"pid %u",*pid);
	// }
}
//0 libres, 1 ocupados
void mostrar_tsl_actualizado(t_list*lista,uint32_t cual_es){
	if(cual_es == 0){
		log_info(log_memoria,"Lista de segmentos libres actualizada");
	}else if(cual_es ==1) {log_info(log_memoria,"Lista de segmentos ocupados actualizada");}
	else log_info(log_memoria,"Tabla de segmentos encontrados donde entra el tamanio solicitado: ");
	uint32_t size = list_size(lista);

	for(int i=0;i<size;i++){
		segmento_t*seg = (segmento_t*)list_get(lista,i);
		//log_info(log_memoria,"BASE %d, SIZE %d",seg->direccion_Base,seg->tamanio);
	}
}

