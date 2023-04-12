#include "sockets.h"

//SERVER
int iniciar_servidor (t_log* logger, char* ip, char* puerto){ //logger donde va a salir el mensaje, ip del server, puerto del server
int socket_servidor;

	struct addrinfo hints, *servinfo; // *p

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

    //establece la ip y el puerto
	getaddrinfo(ip, puerto, &hints, &servinfo);

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	// Asociamos el socket a un puerto
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);
	// Escuchamos las conexiones entrantes
	listen(socket_servidor, SOMAXCONN);

	log_trace(logger, "Listo para escuchar");

    freeaddrinfo(servinfo);

	return socket_servidor;
}


int esperar_cliente(t_log* logger,int socket_servidor)
{

	// Aceptamos un nuevo cliente
	int socket_cliente = accept(socket_servidor, NULL, NULL);
	log_info(logger, "Se conecto un cliente");

	return socket_cliente;
}


// CLIENTE
int crear_conexion(t_log* logger, char *ip, char* puerto)
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


