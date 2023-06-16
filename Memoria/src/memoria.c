#include "memoria.h"
extern t_log* log_memoria;
extern t_config_memoria *cfg;


int main(){


	if(!init() || !cargar_configuracion("memoria.config") || !cargar_memoria()){
		log_error(log_memoria,"Tuviste un error al iniciar capo");
		terminar_memoria();
	}


	//inicio el servidor y acepto mis conexiones
	int server_fd = iniciar_servidor(log_memoria,"MEMORIA","127.0.0.1",cfg->PUERTO_ESCUCHA);



	if(server_escuchar(log_memoria,"MEMORIA",server_fd)==1){
		log_info(log_memoria,"Conexiones establecidas");
	}
	terminar_memoria();
	return EXIT_SUCCESS;
}
