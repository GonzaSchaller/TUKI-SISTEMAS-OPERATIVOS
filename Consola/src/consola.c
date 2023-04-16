#include "consola.h"

int main (){
	int conexion;

	log_consola = log_create("consola.log","consola",1, LOG_LEVEL_DEBUG);

	config_consola = config_create("consola.config");



	puerto = config_get_string_value(config_consola,"PUERTO_KERNEL");
	ip = config_get_string_value(config_consola,"IP_KERNEL");

	conexion = crear_conexion(log_consola,"Kernel", ip, puerto );
	//int conexion2 = crear_conexion(log_consola,"Kernel", ip, puerto );
	terminar_programa(conexion, log_consola, config_consola);

	return EXIT_SUCCESS;
}






