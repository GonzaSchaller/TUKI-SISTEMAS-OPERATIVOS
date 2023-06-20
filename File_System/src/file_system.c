#include "file_system.h"

t_log* logger;
t_config* config;

//char* ip;
//char* puerto_memoria;
//char* puerto_escucha;
//char* superbloque;
//char* bitmap;
//char* bloques;
//char* fcb;
//char* retardo_acceso_bloque;
//int conexion;


void establecer_conexion_kernel(){
	int server_fd = iniciar_servidor(logger, "fileSystem", ip, puerto_escucha);
	log_info(logger, "Servidor listo para recibir cliente");
	int cliente_fd = esperar_cliente(logger, server_fd);
	if(cliente_fd!=-1){
		procesar_peticiones(cliente_fd);
	}
}

void establecer_conexion_memoria(){
	conexion = crear_conexion(logger,"Memoria", ip, puerto_memoria);
}

void establecer_conexiones(){
	pthread_t hilo_memoria, hilo_kernel;

	if(pthread_create(&hilo_kernel, NULL, (void*)establecer_conexion_kernel, NULL)){
		printf("Error al crear el hilo de kernel\n");
		exit(-1);
	}
	if(pthread_create(&hilo_memoria, NULL, (void*)establecer_conexion_memoria, NULL)){
		printf("Error al crear el hilo de memoria\n");
		exit(-2);
	}
	pthread_join(hilo_kernel, NULL);
	pthread_join(hilo_memoria, NULL);
}

int main (){

	// Issue 2967 es donde está la info de por qué definí las variables del .h en el .c también
	logger = log_create("fileSystem.log","FileSystem",1, LOG_LEVEL_DEBUG);



	c = malloc(sizeof(t_config_fs));

	levantar_config();

	//int server_fd = iniciar_servidor(logger, "fileSystem", ip, puerto_fileSystem);

	//log_info(logger , "Servidor listo para recibir cliente");
	//int cliente_fd = esperar_cliente(logger, server_fd);

	//conexion = crear_conexion(logger,"Memoria", ip, puerto_memoria );
	//cuando el cpu es server del kernel
	
	establecer_conexiones();
	cargar_superbloque();

	terminar_programa(conexion, logger, config);

	return EXIT_SUCCESS;

}
