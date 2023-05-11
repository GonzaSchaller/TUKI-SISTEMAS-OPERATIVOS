#include "memoria.h"
extern t_log* log_memoria;
extern t_config_memoria cfg;
extern void* memoria_principal;

int main(){
	if(!init() || !cargar_configuracion() || !cargar_memoria()){
		log_error(log_memoria,"error capo");
		terminar_memoria();
	}
	int server_fd = iniciar_servidor(log_memoria,"MEMORIA","127.0.0.1",cfg->PUERTO_ESCUCHA);
	server_escuchar(log_memoria,"MEMORIA",server_fd);
	terminar_memoria();
	return EXIT_SUCCESS;
}
