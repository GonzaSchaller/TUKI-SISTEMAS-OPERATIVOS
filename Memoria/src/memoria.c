#include "memoria.h"
extern t_log* log_memoria;
extern t_config_memoria *c;


void sighandler(int s){
	terminar_memoria();
	exit(0);
}

int main(){

	signal(SIGINT,sighandler);
	cargar_configuracion("memoria.config");
	cargar_memoria();

	int server_fd = iniciar_servidor(log_memoria,"MEMORIA","192.168.1.35",c->PUERTO_ESCUCHA);
	if(server_escuchar(log_memoria,"MEMORIA",server_fd)==1){
		log_info(log_memoria,"Conexiones establecidas");
	}
	terminar_memoria();
	return EXIT_SUCCESS;
}
