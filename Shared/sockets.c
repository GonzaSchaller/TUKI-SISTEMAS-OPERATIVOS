#include "sockets.h"

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
	int socket_consola = accept(socket_servidor, NULL, NULL);
	log_info(logger, "Se conecto un cliente");

	return socket_consola;
}


