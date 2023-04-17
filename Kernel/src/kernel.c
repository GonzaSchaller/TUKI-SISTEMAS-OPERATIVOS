#include "kernel.h"

//lee el archivo config
void iniciar_config(t_config* config){
	puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
	ip_memoria = config_get_string_value(config,"IP_MEMORIA");
	puerto_memoria = config_get_string_value(config,"PUERTO_MEMORIA");
	//log_info(log_kernel, "Memoria: Lei la ip %s y el puerto %s\n", ip_memoria, puerto_memoria); // hacer con cada uno
	ip_cpu= config_get_string_value(config,"IP_CPU");
	puerto_cpu = config_get_string_value(config,"PUERTO_CPU");
	ip_fileSystem = config_get_string_value(config,"IP_FILESYSTEM");
	puerto_fileSystem = config_get_string_value(config,"PUERTO_FILESYSTEM");
	//ALGORITMO_PLANIFICACION=HRRN
	estimacion_inicial= config_get_int_value(config,"ESTIMACION_INICIAL" );
	//HRRN_ALFA=0.5
	grado_max_multiprogramacion = config_get_int_value(config,"GRADO_MAX_MULTIPROGRAMACION");
	//RECURSOS=[DISCO, RECURSO_1]
	//INSTANCIAS_RECURSOS=[1, 2]
}



void liberarConexiones(int socket1, int socket2, int socket3){
	if(socket1 != -1){
		close(socket1);
	}
	if(socket2 != -1){
		close(socket2);
	}
	if(socket3 != -1){
		close(socket3);
	}
}
void terminar_kernel(t_log* logger,t_config* config){
	if(logger !=NULL){
		log_destroy(logger);
	}
	if(config != NULL){
		config_destroy(config);
	}
}
//TODO
//seguramente el socket_cliente tenga que estar dentor de una estructura
void atender_cliente(void* socket_cliente){ //lo que hago por cada consola conectada
	t_list* listaInstrucciones;
	pcb_t *pcb = malloc(sizeof(pcb_t));


	recv(socket_cliente,listaInstrucciones,sizeof(t_list*),MSG_WAITALL);
	contadorProcesos++;
	inicializarPCB(listaInstrucciones);
	agregarANew(pcb);

	free(socket_cliente);
};
//crea un hilo por cada cliente que se conecta
void escuchar_clientes(t_log* logger, int socket_servidor){

	while (1) {
	   pthread_t thread;
	   int *socket_cliente = malloc(sizeof(int));
	   *socket_cliente = esperar_cliente(logger, socket_servidor); //el server acepta al cliente y devuelve un socket_cliente
	   if(*socket_cliente != -1){ //si se establece la conexion
		   pthread_create(&thread, NULL, (void*) atender_cliente, socket_cliente);
		   pthread_detach(thread);
	   }
	}
}
// TODO
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
//genero y proceso cada conexion
void generar_conexiones(t_log* logger){
	int* conexion1 = malloc(sizeof(int));
	int* conexion2 = malloc(sizeof(int));
	int* conexion3 = malloc(sizeof(int));

	pthread_t thread1, thread2, thread3;
	//creo conexiones
	*conexion1 = crear_conexion(log_kernel, "CPU", ip_cpu, puerto_cpu);
	*conexion2 = crear_conexion(log_kernel, "FileSystem", ip_fileSystem, puerto_fileSystem);
	*conexion3 = crear_conexion(log_kernel, "Memoria", ip_memoria, puerto_memoria);
	//proceso conexiones
	if(*conexion1 != -1){
		pthread_create(&thread1, NULL, (void*) procesar_conexion_cpu, conexion1);
		pthread_detach(thread1);
	}
	if(*conexion2 != -1){
		pthread_create(&thread2, NULL, (void*) procesar_conexion_fileSystem, conexion2);
		pthread_detach(thread2);
	}
	if(*conexion3 != -1){
		pthread_create(&thread3, NULL, (void*) procesar_conexion_memoria, conexion3);
		pthread_detach(thread3);
	}

	liberarConexiones(conexion1, conexion2, conexion3);
}

int main (){
	 	log_kernel = log_create("kernel.log", "Kernel", 1, LOG_LEVEL_DEBUG);
		t_config* config_kernel = config_create("kernel.config");
		iniciar_config(config_kernel);

		int server_fd = iniciar_servidor(log_kernel, "Kernel", ip, puerto_escucha);
		log_info(log_kernel , "Servidor listo para recibir cliente");

		//kernel se conecta a cpu, memoria y fileSystem
		generar_conexiones(log_kernel);

		//atiende los clientes y procesa las tareas de cada uno
		escuchar_clientes(log_kernel, server_fd);

		terminar_kernel(log_kernel, config_kernel);


	    return EXIT_SUCCESS;
}




