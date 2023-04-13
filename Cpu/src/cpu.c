#include "cpu.h"

int main (){
	int conexion;
	char* ip = "127.0.0.1";
	char* ip_memoria;
	char* puerto_cpu;
	char* puerto_memoria;

	//crear un logger
	t_log* logger;
	logger = log_create("cpu.log","Cpu",1,LOG_LEVEL_TRACE);

	//config
	t_config* config;
	config =config_create("cpu.config");

	//lo de la ip y puerto
	ip_memoria = config_get_string_value(config,"IP_MEMORIA");
	puerto_memoria = config_get_string_value(config,"PUERTO_MEMORIA");
	puerto_cpu = config_get_string_value(config,"PUERTO_ESCUCHA");


	int server_fd = iniciar_servidor(logger, "Cpu", ip, puerto_cpu);
	log_info(logger , "Servidor listo para recibir cliente");
	int cliente_fd = esperar_cliente(logger, server_fd);

	//crear conexion para memoria
	conexion = crear_conexion(logger,"Memoria", ip_memoria, puerto_memoria );
	//cuando el cpu es server del kernel


	//problema de index, arreglar

	terminar_programa(conexion, logger, config);

	//para recv()
//	uint32_t handshake = 1;
//	uint32_t result;


	return EXIT_SUCCESS;

}

