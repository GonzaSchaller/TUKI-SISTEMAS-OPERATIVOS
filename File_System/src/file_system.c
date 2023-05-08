#include "file_system.h"

void levantar_config(){
	ip = config_get_string_value(config,"IP_MEMORIA");
	puerto_memoria = config_get_string_value(config,"PUERTO_MEMORIA");
	puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA");
	superbloque = config_get_string_value(config,"PATH_SUPERBLOQUE");
	bitmap = config_get_string_value(config,"PATH_BITMAP");
	bloques = config_get_string_value(config,"PATH_BLOQUES");
	fcb = config_get_string_value(config,"PATH_FCB");
	retardo_acceso_bloque = config_get_string_value(config,"RETARDO_ACCESO_BLOQUE");
}

void establecer_conexion_kernel(){
	int server_fd = iniciar_servidor(logger, "fileSystem", ip, puerto_escucha);
	log_info(logger, "Servidor listo para recibir cliente");
	int cliente_fd = esperar_cliente(logger, server_fd);
}

void establecer_conexion_memoria(){
	conexion = crear_conexion(logger,"Memoria", ip, puerto_memoria);
}

void establecer_conexiones(){
	pthread_t hilo_memoria, hilo_kernel;

	if(pthread_create(&hilo_kernel, NULL, establecer_conexion_kernel(), NULL)){
		printf("Error al crear el hilo de kernel\n");
		exit(-1);
	}
	if(pthread_create(&hilo_memoria, NULL, establecer_conexion_memoria(), NULL)){
		printf("Error al crear el hilo de memoria\n");
		exit(-2);
	}
	pthread_join(hilo_kernel, NULL);
	pthread_join(hilo_memoria, NULL);
}

int main (){
	//int conexion;
	//char* ip = "127.0.0.1";
	//char* puerto_fileSystem, puerto_memoria;

	//crear un logger
	//t_log* logger;
	logger = log_create("fileSystem.log","FileSystem",1, LOG_LEVEL_DEBUG);

	//config
	//t_config* config;
	config =config_create("fileSystem.config");
	/*
	//lo de la ip y puerto
	ip = config_get_string_value(config,"IP_MEMORIA");
	puerto_memoria = config_get_string_value(config,"PUERTO_MEMORIA");
	puerto_fileSystem = config_get_string_value(config,"PUERTO_ESCUCHA");
	*/

	//int server_fd = iniciar_servidor(logger, "fileSystem", ip, puerto_fileSystem);

	//log_info(logger , "Servidor listo para recibir cliente");
	//int cliente_fd = esperar_cliente(logger, server_fd);

	//conexion = crear_conexion(logger,"Memoria", ip, puerto_memoria );
	//cuando el cpu es server del kernel
	
	establecer_conexiones();

	terminar_programa(conexion, logger, config);

	return EXIT_SUCCESS;

}
