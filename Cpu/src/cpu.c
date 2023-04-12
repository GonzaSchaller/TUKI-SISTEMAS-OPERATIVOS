#include "cpu.h"
//#include "shared.c"



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
	//ip = config_get_string_value(config,"IP");
	//puerto = config_get_string_value(config,"PUERTO");

	//crear conexion
	conexion = crear_conexion("127.0.0.1","50150");

	//cuando el cpu es server del kernel
	//int cliente = esperar_cliente();




	//terminar_programa(conexion, logger, config);


	//para recv()
	//uint32_t handshake = 1;
	//uint32_t result;



}


//PASARLO A SHARED

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger;
	nuevo_logger = log_create("CPU.log", "CPU", 1, LOG_LEVEL_INFO);

	if(nuevo_logger == NULL){
		puts("No se pudo crear el logger");
		exit(1);
	}

	return nuevo_logger;
}

int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Ahora vamos a crear el socket.
	int socket_cliente = socket(server_info->ai_family,
								server_info->ai_socktype,
								server_info->ai_protocol);

	// Ahora que tenemos el socket, vamos a conectarlo
	connect(socket_cliente, server_info->ai_addr,server_info->ai_addrlen);

	freeaddrinfo(server_info);

	return socket_cliente;
}

//int esperar_cliente(int socket_servidor)
//{
//	// Quitar esta línea cuando hayamos terminado de implementar la funcion
//	//assert(!"no implementado!");
//
//	// Aceptamos un nuevo cliente
//	int socket_cliente = accept(socket_servidor,NULL, NULL);
//
//	log_info(logger, "Se conecto un cliente!");
//
//	return socket_cliente;
//}
