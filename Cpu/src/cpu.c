#include "cpu.h"
#include "sockets.c"



int main (){
	int conexion;
	char* ip;
	char* puerto;

	//crear un logger
	t_log* logger;
	logger = iniciar_logger();

	//config
	t_config* config;
	config = iniciar_config();

	//lo de la ip y puerto
	ip = config_get_string_value(config,"IP");
	puerto = config_get_string_value(config,"PUERTO");

	//crear conexion para memoria
	conexion = crear_conexion(logger,"Memoria",ip,puerto);

	//cuando el cpu es server del kernel
	//problema de index, arreglar
	int cliente = esperar_cliente(logger,server_fd);

	terminar_programa(server_fd,conexion, logger, config);

	//para recv()
	uint32_t handshake = 1;
	uint32_t result;




}

