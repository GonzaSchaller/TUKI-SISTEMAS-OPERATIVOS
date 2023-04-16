#include "memoria.h"

void terminar_memoria(t_log* logger,t_config* config){
	if(logger !=NULL){
		log_destroy(logger);
	}
	if(config != NULL){
		config_destroy(config);
	}
}

int main (){
	t_log* log_memoria;
	log_memoria = log_create("memoria.log","Memoria",1, LOG_LEVEL_DEBUG);
	//puerto e ip: tirar a otra carpeta
	config_memoria =config_create("memoria.config");

	puerto_memoria = config_get_string_value(config_memoria,"PUERTO_ESCUCHA");

		int server_fd = iniciar_servidor(log_memoria, "Memoria", ip_memoria, puerto_memoria);
		log_info(log_memoria, "Servidor listo para recibir al cliente");
		int cliente_fd = esperar_cliente(log_memoria, server_fd);

		terminar_memoria(log_memoria, config_memoria);


	    return EXIT_SUCCESS;

}


