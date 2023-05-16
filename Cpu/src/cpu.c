#include "cpu.h"
t_log* logger;
int conexion;
void iniciar_config(t_config* config){
    ip = "127.0.0.1";
    puerto_cpu = config_get_string_value(config,"PUERTO_ESCUCHA");
    ip_memoria = config_get_string_value(config,"IP_MEMORIA");
	puerto_memoria = config_get_string_value(config,"PUERTO_MEMORIA");
	retardo_instruccion = config_get_string_value(config,"RETARDO_INSTRUCCION");
    tam_max_segmento = config_get_string_value(config,"TAM_MAX_SEGMENTO");
}
void establecer_conexion_kernel(){
	int server_fd = iniciar_servidor(logger, "Cpu", ip, puerto_cpu);
	log_info(logger, "Servidor listo para recibir cliente");
	int cliente_fd = esperar_cliente(logger, server_fd);
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
	//int conexion;
	//char* ip = "127.0.0.1";
	//char* ip_memoria;
	//char* puerto_cpu;
	//char* puerto_memoria;

	//crear un logger
	//t_log* logger;
	logger = log_create("cpu.log","Cpu",1,LOG_LEVEL_DEBUG);

	//config
	//t_config* config;
	config =config_create("cpu.config");

	//lo de la ip y puerto
	//ip_memoria = config_get_string_value(config,"IP_MEMORIA");
	//puerto_memoria = config_get_string_value(config,"PUERTO_MEMORIA");
	//puerto_cpu = config_get_string_value(config,"PUERTO_ESCUCHA");
	//iniciar_config(config);

	//int server_fd = iniciar_servidor(logger, "Cpu", ip, puerto_cpu);
	//log_info(logger , "Servidor listo para recibir cliente");
	//int cliente_fd = esperar_cliente(logger, server_fd);

	//crear conexion para memoria
	//conexion = crear_conexion(logger,"Memoria", ip_memoria, puerto_memoria );
	//cuando el cpu es server del kernel
	establecer_conexiones();

	//problema de index, arreglar

	terminar_programa(conexion, logger, config);

	//para recv()
//	uint32_t handshake = 1;
//	uint32_t result;


	return EXIT_SUCCESS;

}

