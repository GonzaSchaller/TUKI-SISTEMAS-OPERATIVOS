#include "consola.h"

int main (){
	int consola;

	log_consola = log_create("consola.log","consola",1,LOG_LEVEL_TRACE);

	config_consola = config_create("consola.config");

	log_info(log_consola, "Se creo todo bien");

	puerto = config_get_string_value(config_consola,"PUERTO_KERNEL");
	ip = config_get_string_value(config_consola,"IP_KERNEL");


	log_info(log_consola, "Se sacaron las cosas bien");

	consola = crear_conexion(log_consola,"Kernel", ip, puerto );

	log_info(log_consola, "Se creo la conexion");

	terminar_consola(log_consola, consola, config_consola);

	return EXIT_SUCCESS;
}

void terminar_consola(t_log* log,int conexion, t_config* config){
	log_destroy(log);
	//liberar_conexion(conexion);
	config_destroy(config);
}




