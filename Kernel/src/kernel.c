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

int main (){
	 	log_kernel = log_create("kernel.log", "Kernel", 1, LOG_LEVEL_DEBUG);
		t_config* config_kernel = config_create("kernel.config");
		iniciar_config(config_kernel);

		int server_fd = iniciar_servidor(log_kernel, "Kernel", ip, puerto_escucha);
		log_info(log_kernel , "Servidor listo para recibir cliente");
		int cliente_fd = esperar_cliente(log_kernel, server_fd);




		int conexion1 = crear_conexion(log_kernel, "CPU", ip_cpu, puerto_cpu);
		int conexion2 = crear_conexion(log_kernel, "FileSystem", ip_fileSystem, puerto_fileSystem);
		int conexion3 = crear_conexion(log_kernel, "Memoria", ip_memoria, puerto_memoria);

		terminar_kernel(log_kernel, config_kernel);
		liberarConexiones(conexion1, conexion2, conexion3);

	    return EXIT_SUCCESS;
}

void terminar_kernel(t_log* logger,t_config* config){
	if(logger !=NULL){
		log_destroy(logger);
	}
	if(config != NULL){
		config_destroy(config);
	}
}


