#include "iniciar.h"

t_log*log_memoria;
t_config_memoria *cfg;
void* memoria_principal;
t_list* segmentos_libres;
t_list* segmentos_usados;
segmento_t* (*proximo_hueco) (uint32_t);


uint8_t init() {
	cfg = inicializar_cfg();
   // config = inicializar_configuracion(); //inicializacion del struct de configuracion.
    log_memoria = log_create("memoria.log", "MEMORIA", 1, LOG_LEVEL_INFO);
    log_info(log_memoria,"cree el log");
    return 1;
}

uint8_t cargar_configuracion(char*path){
	t_config*config_memoria;
	config_memoria = config_create(path);
	  if(config_memoria == NULL) {
	        log_error(log_memoria, "No se encontro memoria.config");
	        return 0;
	    }
	  else log_info(log_memoria, "i found it");


		cfg->TAMANIO_MEMORIA = config_get_int_value(config_memoria, "TAM_MEMORIA");


	    cfg->TAMANIO_SEGMENTO = (uint32_t) strtol(config_get_string_value(config_memoria, "TAM_SEGMENTO_0"), NULL, 10);

	    cfg->CANT_SEGMENTOS = config_get_int_value(config_memoria, "CANT_SEGMENTOS");

	    cfg->RETARDO_MEMORIA = config_get_int_value(config_memoria, "RETARDO_MEMORIA");

	    cfg->RETARDO_COMPACTACION = config_get_int_value(config_memoria, "RETARDO_COMPACTACION");

	    //   cfg->IP_MEMORIA = config_get_int_value(config_memoria, "IP_MEMORIA");

	    cfg->ALGORITMO_ASIGNACION = strdup(config_get_string_value(config_memoria, "ALGORITMO_ASIGNACION"));

	    cfg->PUERTO_ESCUCHA = strdup(config_get_string_value(config_memoria, "PUERTO_ESCUCHA"));

	    if(strcmp(cfg->ALGORITMO_ASIGNACION,"BEST") == 0){
	    	proximo_hueco = &proximo_hueco_best_fit;
	    }else if(strcmp(cfg->ALGORITMO_ASIGNACION,"FIRST")==0){
	    	proximo_hueco = &proximo_hueco_first_fit;
	    } else { proximo_hueco = &proximo_hueco_worst_fit;}



	    log_info(log_memoria, "Configuracion cargada correctamente");

	    config_destroy(config_memoria);

	    return 1;


}


uint8_t cargar_memoria(){
	memoria_principal = malloc(cfg->TAMANIO_MEMORIA);
	if (memoria_principal == NULL) log_error(log_memoria, "Fallo en el malloc a memoria_principal"); else log_info(log_memoria,"cargue memoria principal");

	  segmentos_libres = list_create();
	  segmento_t* hueco = new_segmento(0,0,cfg->TAMANIO_MEMORIA);
	  if (hueco == NULL) {
	        log_error(log_memoria, "Fallo en la creacion de t_list* segmentos_libres");
	       // asesinar_seglib(); // borrar la lista creada.
	        return 0;
	      }
	   list_add(segmentos_libres, (void*) hueco);
       segmentos_usados = list_create();

	return 1;
}

void terminar_memoria(){
		log_destroy(log_memoria);
}
