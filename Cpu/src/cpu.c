#include "cpu.h"

int conexion_memoria;

void sighandler(int s){
	terminar_programa(conexion_memoria, logger, config);
	exit(0);
}

void establecer_conexion_kernel(){
	// genera el servidor de cpu
	int server_fd = iniciar_servidor(logger, "Cpu", "192.168.1.21", puerto_cpu);
	log_info(logger, "Servidor listo para recibir cliente");

	// almacena el socket_cliente (kernel)
	int socket_cliente_kernel = esperar_cliente(logger, server_fd);

	while(socket_cliente_kernel != -1){ //para que siga ejecutando
		procesar_instrucciones(socket_cliente_kernel, logger);
	}

}

void establecer_conexion_memoria(){
    conexion_memoria = crear_conexion(logger,"Memoria", ip_memoria, puerto_memoria);
    uint32_t handshake =1;
    uint32_t result;
    send_handshake(conexion_memoria,handshake);
    recv_PC(conexion_memoria,&result);
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

	signal(SIGINT,sighandler);
	logger = log_create("cpu.log","Cpu",1,LOG_LEVEL_DEBUG);
	config = config_create("cpu.config");
	levantar_config();
	//int server_fd = iniciar_servidor(logger, "Cpu", ip, puerto_cpu);
	//log_info(logger , "Servidor listo para recibir cliente");
	//int cliente_fd = esperar_cliente(logger, server_fd);
	//crear conexion para memoria
	//conexion = crear_conexion(logger,"Memoria", ip_memoria, puerto_memoria );
	//cuando el cpu es server del kernel
	establecer_conexiones();


	terminar_programa(conexion_memoria, logger, config);

	return EXIT_SUCCESS;

}
