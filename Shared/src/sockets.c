#include "sockets.h"
//LOG

//t_log* iniciar_logger(void)
//{
//t_log* nuevo_logger = log_create("tp0.log", "Tp0", 1, LOG_LEVEL_INFO);
//if(nuevo_logger == NULL){
//	printf("No se pudo crear el logger\n");
//	exit(1);
//}
//return nuevo_logger;
//}

//SERVER
int iniciar_servidor (t_log* logger, char* nombre_server, char* ip, char* puerto){ //logger donde va a salir el mensaje, nombre server, ip del server, puerto del server
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
	if(socket_servidor == -1){
		log_info(logger, "Error creando el socket del server %s", nombre_server);
	}

	//agrego esto para evitar que a veces no se ponga el servidor en ese socket
	int yes = 1;
	setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
	// Asociamos el socket a un puerto
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);
	// Escuchamos las conexiones entrantes
	listen(socket_servidor, SOMAXCONN);

	log_trace(logger, "Listo para escuchar");

    freeaddrinfo(servinfo);

	return socket_servidor;
}

//server se queda escuchando hasta que se conecta un cliente
int esperar_cliente(t_log* logger,int socket_servidor)
{

	// Aceptamos un nuevo cliente
	int socket_cliente = accept(socket_servidor, NULL, NULL); //acepta creando un nuevo socket
	log_info(logger, "Se conecto un cliente");

	return socket_cliente;
}


 //CLIENTE
int crear_conexion(t_log* logger, char* nombre_server, char *ip, char* puerto) //nombre del server al q se va a conectar
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
	   // Fallo en crear el socket
	    if(socket_cliente == -1) {
	        log_error(logger, "Error creando el socket para %s:%s", ip, puerto);
	        return 0;
	    }

	    // Error conectando
	    if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1) {
	        log_error(logger, "Error al conectar (a %s)\n", nombre_server);
	        freeaddrinfo(server_info);
	        return 0;
	    }
	    else{
	    	log_info(logger, "Cliente conectado en %s:%s (a %s)\n", ip, puerto, nombre_server);;
	    }
	freeaddrinfo(server_info);

	return socket_cliente;
}


//cierra conexion del cliente, log, y config
void terminar_programa(int conexion, t_log* logger, t_config* config)
{
if(logger !=NULL){
	log_destroy(logger);
}
if(config != NULL){
	config_destroy(config);
}
liberar_conexion(conexion);
/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config)
  con las funciones de las commons y del TP mencionadas en el enunciado */
}
//Cierra conexion
void liberar_conexion(int socket_cliente){
	close(socket_cliente);
}

