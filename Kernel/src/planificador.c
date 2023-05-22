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
pthread_mutex_t multiprocesamiento;
sem_t largoPlazo;


void agregarAReady(pcb_t* pcb){
	//log_trace(log_kernel,"Entre en agregar a ready");

	time_t a = time(NULL); //momento en el que entra un proceso, sirve para el HRRN
	pcb->horaDeIngresoAReady = a;
	pthread_mutex_lock(&mutexReady); //para el execute a ready y de blocked a ready y de new a ready

	segmento_t* tseg;
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

void agregarABlock(pcb_t* proceso){
	//log_trace(log_kernel,"Entre en agregar a block ");
	//log_info(log_kernel, "ADSSSS el proceso %d tiene el PC en %d",proceso->PID,proceso->PC);

	pthread_mutex_lock(&mutexBlock);
	list_add(listaBlock, proceso);
	log_info(log_kernel, "[BLOCK] Entra el proceso de PID: %d a la cola.", proceso->PID);
	pthread_mutex_unlock(&mutexBlock);
}


void sacarDeBlock(pcb_t* proceso){

	bool tienenMismoPID(void* elemento){

				if(proceso->PID == ((pcb_t*) elemento)->PID) //se fija si un elemento x de la lista tiene el mismo PID que el proceso q queremos sacar
					return true;
				else
					return false;
			}

	pthread_mutex_lock(&mutexBlock);
	list_remove_by_condition(listaBlock, tienenMismoPID);
	log_info(log_kernel, "[BLOCK] Sale el proceso de PID: %d de la cola.", proceso->PID);
	pthread_mutex_unlock(&mutexBlock);
}

void hiloNew_Ready(){

	while(1){
		sem_wait(&largoPlazo); //ver si sacarlo o no
		pcb_t* proceso = sacarDeNew();
		//log_error(log_kernel,"[===========] agregue a ready desde hilo new ready");
		agregarAReady(proceso);
		// proceso->state = Ready;
	}

}
int tamanioDeListaReady(){

	int tamanio;
	pthread_mutex_lock(&mutexReady);
	tamanio = list_size(listaReady);
	pthread_mutex_unlock(&mutexReady);

	return tamanio;
}

pcb_t* obtener_siguiente_FIFO(){
	pcb_t* procesoPlanificado;

	pthread_mutex_lock(&mutexReady);
	procesoPlanificado = list_remove(listaReady, 0);
    pthread_mutex_unlock(&mutexReady);

	return procesoPlanificado;
}

pcb_t* obtener_siguiente_HRRN(time_t tiempo_actual){
	pcb_t* proceso_planificado= NULL;
	int pos = 0;
	pthread_mutex_lock(&mutexReady);
	int t = list_size(listaReady);
	double max_hrrn = -1;
	for(int i = 0; i < t; i++) {
	    proceso_planificado = list_get(listaReady, i);
	    proceso_planificado->hrrn = (proceso_planificado->estimacion_prox_rafaga + (tiempo_actual - proceso_planificado->horaDeIngresoAReady))/ proceso_planificado->estimacion_prox_rafaga;
	    if(proceso_planificado->hrrn > max_hrrn) {
	      max_hrrn = proceso_planificado->hrrn;
		  pos=i;// Lo importante de este if.
	    }
	  }
	proceso_planificado= list_remove(listaReady,pos); //removemos el elemento con mayor hrrn
    pthread_mutex_unlock(&mutexReady);
    return proceso_planificado;
}


pcb_t* obtener_siguiente_ready(){

//	log_trace(log_kernel,"Entre en obtener sig de ready");
	sem_wait(&contadorReady);

	pcb_t* procesoPlanificado = NULL;

	if (tamanioDeListaReady() > 0){

		// Aca dentro un SWITCH para los distintos algoritmos q llama a una funcion para cada uno
		switch(obtener_algoritmo_planificacion(algoritmo_planificacion)){
			//	log_trace(log_kernel,"EL ALGORITMO DE PLANIF ES: %d",algoritmo_config);
				case FIFO:
					procesoPlanificado = obtener_siguiente_FIFO();
				break;
				case HRRN:
					time_t b= time(NULL);
					procesoPlanificado = obtener_siguiente_HRRN(b); //calcular el hrrn segun el tiempo final y elimina el de mayor RR
				break;
			  }


	}

	// Devuelve NULL si no hay nada en ready
	// Caso contrario devuelve el que tiene mas prioridad segun el algoritmo que se este empleando
	return procesoPlanificado;
}
void* hiloReady_Execute(){
	uint32_t pc, tiempo_bloqueo_kernel;
	while(1){
		pthread_mutex_lock(&multiprocesamiento);
		pcb_t* pcb_siguiente = obtener_siguiente_ready();

		enviar_pcb_cpu(conexion_cpu, pcb_siguiente); // lo estamos mandando a exe
		pcb_siguiente->horaDeIngresoAExe = ((float) time(NULL))*1000;

			if (!recv_PC(conexion_cpu, &pc)) {
				log_error(log_kernel, "Fallo recibiendo pc");
			}
			pcb_siguiente->PC = pc;

			if (!recv_tiempo_bloqueante(conexion_cpu, &tiempo_bloqueo_kernel)) { //tiempo que pasar en block
				log_error(log_kernel, "Fallo recibiendo el tiempo bloqueante");
			}

			pcb_siguiente->tiempo_bloqueo = tiempo_bloqueo_kernel;

			time_t fin_exe = time(NULL);
			float tiempoDeFin = ((float) fin_exe)*1000; // el 1000?
			pcb_siguiente->rafaga_anterior_real =pcb_siguiente-> horaDeIngresoAExe - tiempoDeFin;//pcb_siguiente-> horaDeSalidaDeExe;
			pcb_siguiente->estimacion_prox_rafaga = (hrrn_alfa* pcb_siguiente->rafaga_anterior_real)+ ((1-hrrn_alfa)* pcb_siguiente-> estimacion_rafaga_anterior);
			pcb_siguiente->estimacion_rafaga_anterior = pcb_siguiente->estimacion_prox_rafaga;

			if(pcb_siguiente->tiempo_bloqueo > 0){// caso bloqueo
				agregarABlock(pcb_siguiente);
				sem_post(&multiprogramacion); //le digo al new que ya puede mandar otro proceso mientras el grado de multiprog sea > 0
				}
			else
			{
				if(pcb_siguiente->PC < list_size(pcb_siguiente->instrucciones)){ // caso YIELD
					agregarAReady(pcb_siguiente);
				}
				else{// caso EXIT o error
					//terminarEjecucion(pcb_siguiente);
					sem_post(&multiprogramacion); //le digo al new que ya puede mandar otro proceso mientras el grado de multiprog sea > 0
				}
			}

		pthread_mutex_unlock(&multiprocesamiento);
	}

}


	//procesoPlanificado->rafaga_anterior_real =procesoPlanificado-> horaDeIngresoAExe - procesoPlanificado-> horaDeSalidaDeExe;
	//procesoPlanificado->estimacion_prox_rafaga = (hrrn_alfa* procesoPlanificado->rafaga_anterior_real)+ ((1+hrrn_alfa)* procesoPlanificado-> estimacion_rafaga_anterior);
	//procesoPlanificado->estimacion_rafaga_anterior = procesoPlanificado->estimacion_prox_rafaga;
	//time_t final_ready = time(NULL);
