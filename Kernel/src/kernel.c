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
} //QUEDA
void inicializar_semaforos(){

	pthread_mutex_init(&mutexNew, NULL);
	pthread_mutex_init(&mutexReady, NULL);
	pthread_mutex_init(&mutexBlock, NULL);
	pthread_mutex_init(&mutexExe, NULL);
	pthread_mutex_init(&mutexExit, NULL);

	sem_init(&contadorNew, 0, 0); // Estado New
	sem_init(&contadorReady, 0, 0); // Estado Ready
	sem_init(&contadorExe, 0, 0); // Estado Exe
	//sem_init(&contadorProcesosEnMemoria, 0, 0);    // Memoria IMP HAY QUE VER COMO SE INICIALIZA PORQUE ESTO AFECTA LA DISPONIBILIDAD DE LA COLA READY
	sem_init(&multiprogramacion, 0, grado_max_multiprogramacion); // hasta 4 procesos en ready
	//pthread_mutex_init(&multiprocesamiento, NULL);
	sem_init(&contadorBlock, 0, 0);
	sem_init(&largoPlazo, 0, 1);


	//sem_init(&hilo_sincro_cpu_kernel, 0, 0);
}

void inicializar_listas(){


	colaNew = queue_create();
	listaReady = list_create();
	listaExe = list_create();
	listaBlock = list_create();
	listaExit = list_create();

	//lista_instrucciones_kernel = list_create();
	//lista_pcb_en_memoria = list_create();

}

void destruir_semaforos_listas(){

    list_destroy_and_destroy_elements(listaExe,free);
    list_destroy_and_destroy_elements(listaBlock,free);

   // list_destroy_and_destroy_elements(listaExit,free);
    list_destroy_and_destroy_elements(listaReady,free);
    //list_destroy_and_destroy_elements(lista_instrucciones_kernel,free);
    //list_destroy_and_destroy_elements(lista_pcb_en_memoria,free);
    queue_destroy_and_destroy_elements(colaNew,free);


    pthread_mutex_destroy(&mutexBlock);
    pthread_mutex_destroy(&mutexExe);
    pthread_mutex_destroy(&mutexExit);
    pthread_mutex_destroy(&mutexNew);

}
//void inicializar_planificacion(){
//
//	pthread_create(&hiloNewReady, NULL, (void*)hiloNew_Ready, NULL);
//	pthread_create(&hiloReady_Exec, NULL, (void*)hiloReady_Exe, NULL);
//	pthread_detach(hiloNewReady);
//	pthread_detach(hiloReady_Exec);
//
//
//}


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
}//QUEDA
void terminar_kernel(t_log* logger,t_config* config){
	if(logger !=NULL){
		log_destroy(logger);
	}
	if(config != NULL){
		config_destroy(config);
	}
}
//Pasar esta funcion para mi a conexiones_kernel
//se conecta a cpu, memoria, fileSystem y crea los hilos para procesar las conexiones
void generar_conexiones(t_log* log_kernel){
	int conexion1;
	int conexion2;
	int conexion3;

	pthread_t thread1, thread2, thread3;
	//creo conexiones
	conexion1 = crear_conexion(log_kernel, "CPU", ip_cpu, puerto_cpu);
	conexion2 = crear_conexion(log_kernel, "FileSystem", ip_fileSystem, puerto_fileSystem);
	conexion3 = crear_conexion(log_kernel, "Memoria", ip_memoria, puerto_memoria);
	//proceso conexiones
	if(conexion1 != -1){
		pthread_create(&thread1, NULL, (void*) procesar_conexion_cpu, &conexion1);
		pthread_detach(thread1);
	}
	if(conexion2 != -1){
		pthread_create(&thread2, NULL, (void*) procesar_conexion_fileSystem, &conexion2);
		pthread_detach(thread2);
	}
	if(conexion3 != -1){
		pthread_create(&thread3, NULL, (void*) procesar_conexion_memoria, &conexion3);
		pthread_detach(thread3);
	}

	liberarConexiones(conexion1, conexion2, conexion3);
}


int main (){
		ip = "127.0.0.1";
	 	t_log* log_kernel = log_create("kernel.log", "Kernel", 1, LOG_LEVEL_DEBUG);
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




