#include "iniciar.h"
#define POZO 9999

t_log*log_memoria;
t_config_memoria *cfg;

void* memoria_principal;
int memoria_disponible;

t_list* segmentos_libres;
t_list* segmentos_ocupados;
segmento_t* (*proximo_hueco) (uint32_t);
segmento_t* segmento_0;

t_list* lista_de_pids;

void cargar_configuracion(char*path){
	cfg = malloc(sizeof(t_config_memoria));
	log_memoria = log_create("memoria.log", "MEMORIA", 1, LOG_LEVEL_INFO);
	//log_info(log_memoria,"cree el log");


	t_config*config_memoria;
	config_memoria = config_create(path);
	  if(config_memoria == NULL) {
	        log_error(log_memoria, "No se encontro memoria.config");
	    }

		cfg->TAMANIO_MEMORIA = config_get_int_value(config_memoria, "TAM_MEMORIA");
	    cfg->TAMANIO_SEGMENTO_0 = config_get_int_value(config_memoria, "TAM_SEGMENTO_0");
	    cfg->CANT_SEGMENTOS = config_get_int_value(config_memoria, "CANT_SEGMENTOS");
	    cfg->RETARDO_MEMORIA = config_get_int_value(config_memoria, "RETARDO_MEMORIA");
	    cfg->RETARDO_COMPACTACION = config_get_int_value(config_memoria, "RETARDO_COMPACTACION");

	    cfg->ALGORITMO_ASIGNACION = strdup(config_get_string_value(config_memoria, "ALGORITMO_ASIGNACION"));
	    cfg->PUERTO_ESCUCHA = strdup(config_get_string_value(config_memoria, "PUERTO_ESCUCHA"));

	    if(strcmp(cfg->ALGORITMO_ASIGNACION,"BEST") == 0){
	    	proximo_hueco = &proximo_hueco_best_fit;
	    	log_info(log_memoria,"%s",cfg->ALGORITMO_ASIGNACION);
	    }else if(strcmp(cfg->ALGORITMO_ASIGNACION,"FIRST")==0){
	    	proximo_hueco = &proximo_hueco_first_fit;
	    	log_info(log_memoria,"%s",cfg->ALGORITMO_ASIGNACION);
	    } else if (strcmp(cfg->ALGORITMO_ASIGNACION,"WORST")==0){
	    	proximo_hueco = &proximo_hueco_worst_fit;
	    	log_info(log_memoria,"%s",cfg->ALGORITMO_ASIGNACION);
	    }


	    config_destroy(config_memoria);
}


void cargar_memoria(){
	  memoria_principal = malloc(cfg->TAMANIO_MEMORIA);


	  if (memoria_principal == NULL) log_error(log_memoria, "Fallo en el malloc a memoria_principal"); else log_info(log_memoria,"cargue memoria principal");

	  memset(memoria_principal,0,cfg->TAMANIO_MEMORIA);

	  segmentos_ocupados = list_create();
	  segmentos_libres = list_create();

	  segmento_0 = new_segmento(0,0,cfg->TAMANIO_SEGMENTO_0,POZO);

	  segmento_t* hueco = new_segmento(0,cfg->TAMANIO_SEGMENTO_0,cfg->TAMANIO_MEMORIA-cfg->TAMANIO_SEGMENTO_0,POZO); // primero creo el hueco.

	  if (hueco == NULL) {
	        log_error(log_memoria, "falle creando el hueco libre de tamanio de memoria inicial");
	        list_destroy(segmentos_libres);
	  }
	   list_add(segmentos_libres, (void*) hueco);

	   memoria_disponible = cfg->TAMANIO_MEMORIA - cfg->TAMANIO_SEGMENTO_0;

	   lista_de_pids = list_create();

	   list_add(segmentos_ocupados,(void*) segmento_0);
}

void terminar_memoria(){
		log_destroy(log_memoria);


		list_destroy_and_destroy_elements(segmentos_libres, (void*) free);
		list_destroy_and_destroy_elements(segmentos_ocupados, (void*) free);

		list_destroy_and_destroy_elements(lista_de_pids, (void*) free);

	    free(cfg);
	    free(memoria_principal);
	    finalizar_mutex();

}
