#include "kernel.h"

t_log* log_kernel;
int conexion_cpu;
int conexion_fileSystem;
int conexion_memoria;
t_config* config_kernel;
int server_kernel;


char* estado_pcb_a_string(uint32_t estado_int){// CAMBIE
	  switch (estado_int) {
	        case NEW:
	            return "NEW";
	        case READY:
	            return "READY";
	        case EXEC:
	            return "EXEC";
	        case BLOCK:
	            return "BLOCK";
	        case FINISH:
	        	return "EXIT";
	        default:
	            return "Registro desconocido";
	    }
}


//lee el archivo config
void iniciar_config(t_config* config){
	ip = config_get_string_value(config, "IP");;
	puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
	ip_memoria = config_get_string_value(config,"IP_MEMORIA");
	puerto_memoria = config_get_string_value(config,"PUERTO_MEMORIA");
	//log_info(log_kernel, "Memoria: Lei la ip %s y el puerto %s\n", ip_memoria, puerto_memoria); // hacer con cada uno
	ip_cpu= config_get_string_value(config,"IP_CPU");
	puerto_cpu = config_get_string_value(config,"PUERTO_CPU");
	ip_fileSystem = config_get_string_value(config,"IP_FILESYSTEM");
	puerto_fileSystem = config_get_string_value(config,"PUERTO_FILESYSTEM");
	algoritmo_planificacion= config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	estimacion_inicial= config_get_int_value(config,"ESTIMACION_INICIAL" );
	hrrn_alfa = (float) config_get_double_value(config, "HRRN_ALFA");
	validar_alfa(hrrn_alfa);
	grado_max_multiprogramacion = config_get_int_value(config,"GRADO_MAX_MULTIPROGRAMACION");
	lista_recursos = obtener_recursos(config, "RECURSOS", "INSTANCIAS_RECURSOS");
} //QUEDA
void inicializar_semaforos(){

	pthread_mutex_init(&mutexNew, NULL);
	pthread_mutex_init(&mutexReady, NULL);
	pthread_mutex_init(&mutexExe, NULL);
	pthread_mutex_init(&mutexExit, NULL);
	pthread_mutex_init(&multiprocesamiento, NULL);
	pthread_mutex_init(&mutexHiloTruncate,NULL);
	pthread_mutex_init(&mutexHiloRead,NULL);
	pthread_mutex_init(&mutexHiloWrite,NULL);


	sem_init(&contadorNew, 0, 0); // Estado New
	sem_init(&contadorReady, 0, 0); // Estado Ready
	sem_init(&multiprogramacion, 0, grado_max_multiprogramacion); // hasta 4 procesos en ready
	sem_init(&semFWrite,0, 1);
	sem_init(&semFRead,0,1);
	sem_init(&largoPlazo, 0, 0);


	//sem_init(&hilo_sincro_cpu_kernel, 0, 0);
}
void inicializar_listas(){


	colaNew = queue_create();
	listaReady = list_create();
	listaExe = list_create();
	listaBlock = list_create();
	tabla_ArchivosAbiertosGlobal = list_create();
	lista_total_procesos = list_create();
	//lista_recursos = list_create();
	//lista_instrucciones_kernel = list_create();
	//lista_pcb_en_memoria = list_create();

}

void iniciar_planificacion(){
	inicializar_listas();
	inicializar_semaforos();
	pthread_t hiloNewReady;
	pthread_t hiloReadyExecute;
	pthread_create(&hiloReadyExecute, NULL,(void*)hiloReady_Execute, NULL);
	pthread_detach(hiloReadyExecute);
	pthread_create(&hiloNewReady, NULL, (void*)hiloNew_Ready, NULL);
	pthread_detach(hiloNewReady);

}


void destruir_semaforos_listas(){

    list_destroy_and_destroy_elements(listaExe,free);
    list_destroy_and_destroy_elements(listaBlock,free);

    list_destroy_and_destroy_elements(listaReady,free);
    //list_destroy_and_destroy_elements(lista_instrucciones,free);
    //list_destroy_and_destroy_elements(lista_pcb_en_memoria,free);
    queue_destroy_and_destroy_elements(colaNew,free);
	list_destroy(lista_total_procesos);


	for(int i;i<list_size(tabla_ArchivosAbiertosGlobal);i++){
		fcb_kernel* archivo = list_remove(tabla_ArchivosAbiertosGlobal,i);
		free(archivo->colaBloqueados);
		free(archivo);
	}
    list_destroy(tabla_ArchivosAbiertosGlobal);
	for(int i;i<list_size(lista_recursos);i++){
		recurso_sistema* recurso = list_remove(lista_recursos,i);
		free(recurso->colaBloqueados);
		free(recurso);
	}
	list_destroy(lista_recursos);

    pthread_mutex_destroy(&mutexNew);
    pthread_mutex_destroy(&mutexReady);
    pthread_mutex_destroy(&mutexExit);
    pthread_mutex_destroy(&multiprocesamiento);
    pthread_mutex_destroy(&mutexHiloTruncate);
    pthread_mutex_destroy(&mutexHiloRead);
    pthread_mutex_destroy(&mutexHiloWrite);

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

void terminar_kernel(t_config* config, int server_kernel){
	if(log_kernel !=NULL){
		log_destroy(log_kernel);
	}
	if(config != NULL){
		config_destroy(config);
	}
		close(server_kernel);

}

void sighandler(int s){
	terminar_kernel(config_kernel, server_kernel);
	liberarConexiones(conexion_fileSystem, conexion_fileSystem, conexion_memoria);
	destruir_semaforos_listas();

	exit(0);
}

//Pasar esta funcion para mi a conexiones_kernel
//se conecta a cpu, memoria, fileSystem y crea los hilos para procesar las conexiones
void generar_conexiones(){
	conexion_cpu = crear_conexion(log_kernel, "CPU", ip_cpu, puerto_cpu);
	conexion_fileSystem = crear_conexion(log_kernel, "FileSystem", ip_fileSystem, puerto_fileSystem);
	conexion_memoria = crear_conexion(log_kernel, "Memoria", ip_memoria, puerto_memoria);
}

int server_escuchar(int server_kernel){
   // seamforo cantidad consolas max
	int consola_socket = esperar_cliente(log_kernel, server_kernel);
	if(consola_socket != -1){
		pthread_t hilo;
		args_atender_cliente* args = malloc(sizeof(args_atender_cliente));
		args->log = log_kernel;
		args->socket = consola_socket;
		args->server_name = "Kernel";
		//procesar_conexion_consola((void*) args); //el que descomenta esto lo mato
		pthread_create(&hilo, NULL, (void*) procesar_conexion_consola, (void*) args);
		pthread_detach(hilo);
		// wait semaforo 1
		// post semaforo consola
		return 1;
	}
	return 0;
}
//void sighandler(int s){
//	cerrar
//}

int main (){

		signal(SIGINT,sighandler);
	 	log_kernel = log_create("kernel.log", "Kernel", 1, LOG_LEVEL_DEBUG);
	 	config_kernel = config_create("kernel.config");
		iniciar_config(config_kernel);

		server_kernel = iniciar_servidor(log_kernel, "Kernel","192.168.1.21" , puerto_escucha);
		log_info(log_kernel , "Servidor listo para recibir cliente");



		//kernel se conecta a cpu, memoria y fileSystem
		generar_conexiones();
		iniciar_planificacion();
		//atiende los clientes y procesa las tareas de cada uno
		while(server_escuchar(server_kernel));


		terminar_kernel(config_kernel, server_kernel);
		liberarConexiones(conexion_fileSystem, conexion_fileSystem, conexion_memoria);
		destruir_semaforos_listas();

	    return EXIT_SUCCESS;
}




