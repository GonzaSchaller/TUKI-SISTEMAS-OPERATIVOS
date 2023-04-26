#include "planificador.h"


t_queue* colaNew;
t_list* listaReady;
t_list* listaExe;
t_list* listaBlock;
t_list* listaExit;

pthread_mutex_t mutexNew;
pthread_mutex_t mutexReady;
pthread_mutex_t mutexBlock;
pthread_mutex_t mutexExe;
pthread_mutex_t mutexExit;

sem_t contadorNew;
sem_t contadorReady;
sem_t contadorExe;
sem_t contadorBlock;
//sem_t contadorProcesosEnMemoria;
sem_t multiprogramacion;
//pthread_mutex_t multiprocesamiento;
sem_t largoPlazo;


void agregarAReady(pcb_t* pcb){ // Tendria mas sentido que log_kernel no sea pasado por parametro
	//log_trace(log_kernel,"Entre en agregar a ready");

	//time_t a = time(NULL); //momento en el que entra un proceso, sirve para el HRRN

	pthread_mutex_lock(&mutexReady); //para el execute a ready y de blocked a ready y de new a ready

	tabla_segmentos* tseg;
	list_add(listaReady, pcb); //agrega a la listaReady

	log_info(log_kernel, "[READY] Entra el proceso de PID: %d a la cola.", pcb->PID);
	send_INICIAR_ESTRUCTURA_MEMORIA(conexion_memoria, "Inicializa las estructuras"); //mandamos mensaje a memoria que incie sus estructuras
	if(!recv_TABLA_SEGMENTOS(conexion_memoria, &tseg)){ //recibimos la direccion de la tabla de segmento
		log_info(log_kernel, "No se recibio tabla de segmentos para el proceso de PID: %d", pcb->PID);
	}
	pcb->TSegmento = *tseg;

	//printf("PROCESOS EN READY: %d \n", list_size(colaReady));
	log_debug(log_kernel,"[----------------PROCESOS EN READY: %d --------------------]\n", list_size(listaReady)); //max 4

	pthread_mutex_unlock(&mutexReady);
	sem_post(&contadorReady);
}

//estado Ready
void hiloNew_Ready(){

	while(1){
		sem_wait(&largoPlazo); //ver si sacarlo o no
		pcb_t* proceso = sacarDeNew();
		//proceso->estimacionAnterior = estimacion_inicial;
		//proceso->estimacionActual = estimacion_inicial;	//"estimacio_inicial" va a ser una variable que vamos a obtener del cfg
		sem_wait(&multiprogramacion); // poner sem_post en el sacar_ready
		//log_error(log_kernel,"[===========] agregue a ready desde hilo new ready");
		agregarAReady(proceso);

	}

}


