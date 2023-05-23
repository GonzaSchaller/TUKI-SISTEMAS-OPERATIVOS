#include "memoria.h"
extern t_log* log_memoria;
extern t_config_memoria *cfg;
extern void* memoria_principal;

int main(){
	printf("dsadsa");

	if(!init() || !cargar_configuracion("memoria.config")){
		log_error(log_memoria,"Tuviste un error al iniciar capo");
		terminar_memoria();
	}


	//inicio el servidor y acepto mis conexiones
	int server_fd = iniciar_servidor(log_memoria,"MEMORIA","127.0.0.1",cfg->PUERTO_ESCUCHA);
	if(server_escuchar3(log_memoria,"MEMORIA",server_fd)){
		log_info(log_memoria,"Conexiones establecidas");
		cargar_memoria();
	}


	terminar_memoria();

	return EXIT_SUCCESS;
}
