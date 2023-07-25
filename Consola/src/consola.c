#include "consola.h"



void levantar_config(){
	puerto_kernel = config_get_string_value(config_consola,"PUERTO_KERNEL");
	ip_kernel = config_get_string_value(config_consola,"IP_KERNEL");
}

int main (int argc, char* argv[]){
	if(argc != 3){
		printf("Cantidad de parametros incorrecta");
		return EXIT_FAILURE;
	}
	char* path;
	char* config;

	log_consola = log_create("consola.log","consola",1, LOG_LEVEL_DEBUG);
	config = argv[1]; //path1
	config_consola = config_create(config);
	levantar_config();

	path = argv[2]; //path 2 pseudocodigo

	lista_instrucciones = list_create();
	//int cant_instrucciones = list_size(lista_instrucciones); // deberiamos mandar la cantidad de instrucciones y despues la lista_instrucciones

	obtener_instrucciones(path);

	conexion_kernel = crear_conexion(log_consola,"Kernel", ip_kernel, puerto_kernel);
	if(conexion_kernel == -1){
		return EXIT_FAILURE;
		terminar_programa(conexion_kernel, log_consola, config_consola);
	}

	send_instrucciones_a_kernel();
	//int conexion2 = crear_conexion(log_consola,"Kernel", ip, puerto );
	uint32_t pid;
	if(!recv_PID(conexion_kernel, &pid)){
		log_info(log_consola, "Error recibiendo exit");
		return EXIT_FAILURE;
	}
	log_info(log_consola, "Finaliza Consola PID: <%d>", pid);
	terminar_programa(conexion_kernel, log_consola, config_consola);
	return EXIT_SUCCESS;

}

// TODO nos falta poner la cantidad de consolas que pueden estar activas, para mi semaforo, para gonza contador
// TODO revisar lo de recibir terminar ejecucion.


