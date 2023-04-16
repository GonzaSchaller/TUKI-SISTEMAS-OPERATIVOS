#include "file_system.h"

int main (){
	int conexion;
	char* ip = "127.0.0.1";
	char* puerto_fileSystem, puerto_memoria;

	//crear un logger
	t_log* logger;
	logger = log_create("fileSystem.log","FileSystem",1, LOG_LEVEL_DEBUG);

	//config
	t_config* config;
	config =config_create("fileSystem.config");

	//lo de la ip y puerto
	ip = config_get_string_value(config,"IP_MEMORIA");
	puerto_memoria = config_get_string_value(config,"PUERTO_MEMORIA");
	puerto_fileSystem = config_get_string_value(config,"PUERTO_ESCUCHA");


	int server_fd = iniciar_servidor(logger, "fileSystem", ip, puerto_fileSystem);

	log_info(logger , "Servidor listo para recibir cliente");
	int cliente_fd = esperar_cliente(logger, server_fd);

	conexion = crear_conexion(logger,"Memoria", ip, puerto_memoria );
	//cuando el cpu es server del kernel


	//problema de index, arreglar

	terminar_programa(conexion, logger, config);

	//para recv()
//	uint32_t handshake = 1;
//	uint32_t result;


	return EXIT_SUCCESS;

}
