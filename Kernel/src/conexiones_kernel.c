#include "conexiones_kernel.h"
#include "planificador.h"


void escuchar_clientes(t_log* logger, int socket_servidor){

	args_atender_cliente* args = malloc(sizeof(args_atender_cliente));
	args->log = logger;
	args->socket = socket_servidor;
	args->server_name = "Kernel";
	while (1) {
	   pthread_t thread;
	   int *socket_cliente = malloc(sizeof(int));
	   *socket_cliente = esperar_cliente(logger, socket_servidor); //el server acepta al cliente y devuelve un socket_cliente
	   if(*socket_cliente != -1){ //si se establece la conexion
		   pthread_create(&thread, NULL, (void*) atender_cliente, (void*) args);
		   pthread_detach(thread);
	   }
	}
}



void procesar_conexion_cpu(void* socket_cliente){

	free(socket_cliente);
}
// TODO
void procesar_conexion_fileSystem(void* socket_cliente){

	free(socket_cliente);
}
// TODO
void procesar_conexion_memoria(void* socket_cliente){

	free(socket_cliente);
}

