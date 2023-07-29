#include "planificador.h"

t_queue* colaNew;
t_list* listaReady;
t_list* listaExe;
t_list* listaBlock;
t_list* listaExit;
t_list* tabla_ArchivosAbiertosGlobal;
t_list* lista_total_procesos;

pthread_mutex_t mutexNew;
pthread_mutex_t mutexReady;
pthread_mutex_t mutexExe;
pthread_mutex_t mutexExit;
pthread_mutex_t mutexHiloTruncate;
pthread_mutex_t mutexHiloRead;
pthread_mutex_t mutexHiloWrite;
pthread_mutex_t multiprocesamiento;

sem_t contadorNew;
sem_t contadorReady;
sem_t semFWrite;
sem_t semFRead;
sem_t multiprogramacion;
sem_t largoPlazo;

void print_lista_PID(){
	pcb_t* proceso_planificado= NULL;
	char mensaje[100] = "Cola Ready < ";
	strcat(mensaje,algoritmo_planificacion);
	char* mensaje2 =" >: [";
	strcat(mensaje,mensaje2);

	for(int i = 0; i < list_size(listaReady); i++) {
		if (i != 0) {
		            strcat(mensaje, ", ");
		        }
	    proceso_planificado = list_get(listaReady, i);
	    char pid_str[10];
	    sprintf(pid_str, "%d", proceso_planificado->contexto.PID);
	    strcat(mensaje, pid_str);
	  }
	strcat(mensaje, "]");
	log_info(log_kernel,"%s",mensaje);
}


void agregarNewAReady(pcb_t* pcb){
	//log_trace(log_kernel,"Entre en agregar a ready");
	time_t a = time(NULL) * 1000; //momento en el que entra un proceso, sirve para el HRRN
	pcb->horaDeIngresoAReady = a;
	pthread_mutex_lock(&mutexReady); //para el execute a ready y de blocked a ready y de new a ready

	list_add(listaReady, pcb); //agrega a la listaReady
	send_INICIAR_ESTRUCTURA_MEMORIA(conexion_memoria); //mandamos mensaje a memoria que incie sus estructuras
	send_PID(conexion_memoria,pcb->contexto.PID); //mandamos a memoria pid para que lo asigne a cada segmento
	if(recv_TABLA_SEGMENTOS(conexion_memoria, &pcb->contexto.TSegmento)){ //recibimos la direccion de la tabla de segmento
		segmento_t* s=list_get(pcb->contexto.TSegmento,0);
		//log_info(log_kernel, "Segmento : %d, %d, %d, %d", s->direccion_Base, s->id, s->pid, s->tamanio);
	}
	else{
		log_error(log_kernel, "No se recibio tabla de segmentos para el proceso de PID: %d", pcb->contexto.PID);
	}
	pcb->state_anterior = pcb->state;
	pcb->state = READY;
	log_info(log_kernel, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <%s>",pcb->contexto.PID,estado_pcb_a_string(pcb->state_anterior),estado_pcb_a_string(pcb->state));
	print_lista_PID();
	pthread_mutex_unlock(&mutexReady);
	sem_post(&contadorReady);
}

void agregarAReady(pcb_t* pcb){
	time_t llegadaReady = time(NULL) * 1000;
	pcb->horaDeIngresoAReady = llegadaReady;
	pthread_mutex_lock(&mutexReady);
	list_add(listaReady, pcb);
	pcb->state_anterior = pcb->state;
	pcb->state = READY;
	log_info(log_kernel, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <%s>",pcb->contexto.PID,estado_pcb_a_string(pcb->state_anterior),estado_pcb_a_string(pcb->state));
	print_lista_PID();
	pthread_mutex_unlock(&mutexReady);
	sem_post(&contadorReady);

}

void hiloNew_Ready(){

	while(1){
		sem_wait(&largoPlazo); //ver si sacarlo o no
		sem_wait(&multiprogramacion);
		pcb_t* proceso = sacarDeNew();
		//log_error(log_kernel,"[===========] agregue a ready desde hilo new ready");
		agregarNewAReady(proceso);
		//log_info(log_kernel, "Numero de instrucciones: <%d> ",list_size(proceso->instrucciones));
	}

}
int tamanioDeListaReady(){

	int tamanio = 0;
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
					time_t b= time(NULL) * 1000;
					procesoPlanificado = obtener_siguiente_HRRN(b); //calcular el hrrn segun el tiempo final y elimina el de mayor RR
				break;
			  }
	}
	// Devuelve NULL si no hay nada en ready
	// Caso contrario devuelve el que tiene mas prioridad segun el algoritmo que se este empleando
	return procesoPlanificado;
}


void recalcular_rafagas_HRRN(pcb_t* pcb_siguiente, float tiempoDeFin){
	uint32_t algoritmo_planif;

	algoritmo_planif = obtener_algoritmo_planificacion(algoritmo_planificacion);
	if(algoritmo_planif == HRRN){
				pcb_siguiente->rafaga_anterior_real =pcb_siguiente-> horaDeIngresoAExe - tiempoDeFin;//pcb_siguiente-> horaDeSalidaDeExe;
				pcb_siguiente->estimacion_rafaga_anterior = pcb_siguiente->estimacion_prox_rafaga;
				pcb_siguiente->estimacion_prox_rafaga = (hrrn_alfa* pcb_siguiente->rafaga_anterior_real)+ ((1-hrrn_alfa)* pcb_siguiente-> estimacion_rafaga_anterior);
				}
}


void hiloReady_Execute(){

	while(1)
	{
		pthread_mutex_lock(&multiprocesamiento);
		pcb_t* pcb_siguiente = obtener_siguiente_ready();
		contexto_ejecucion contexto;
		contexto.TSegmento = list_create();
	//	segmento_t *segmento = list_get(pcb_siguiente->contexto.TSegmento, 0);
		pcb_siguiente->state_anterior = pcb_siguiente->state;
		pcb_siguiente->state = EXEC;
		log_info(log_kernel, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <%s>",pcb_siguiente->contexto.PID,estado_pcb_a_string(pcb_siguiente->state_anterior),estado_pcb_a_string(pcb_siguiente->state));
		enviar_pcb_cpu(conexion_cpu, pcb_siguiente); // lo estamos mandando a exe
		log_info(log_kernel, "-------- Running PID: <%d> --------", pcb_siguiente->contexto.PID);
		pcb_siguiente->horaDeIngresoAExe = ((float) time(NULL)) * 1000;
		noSalePorIO = true;
		while(pcb_siguiente->state == EXEC && noSalePorIO){ //que ejecute cada instruccion hasta que cambie de estado
			manejar_contextosDeEjecucion(pcb_siguiente, contexto, contexto.TSegmento);
		 	 }
		pthread_mutex_unlock(&multiprocesamiento);
	}

  }

void liberar_Recursos(pcb_t* pcb) { //para liberar recursos asignados de un proceso //todo revisar como se libera recursos en pruebaMemoria, habria que liberar cuando ya no va a ejecutar mas el kernel, no por cada pcb
    for (int i = 0; i < list_size(pcb->recursos_asignados); i++) {
        recurso_sistema* recurso_del_pcb = list_get(pcb->recursos_asignados, i);
        recurso_sistema* recurso = encontrar_recurso(lista_recursos, recurso_del_pcb->nombre);
        recurso->instancia += recurso_del_pcb->instancia;
      //  queue_destroy_and_destroy_elements(recurso->colaBloqueados,free);
	}
}
void terminarEjecucion(pcb_t* pcb){
	pthread_mutex_lock(&mutexExit);
	liberar_Recursos(pcb); //todo comentado

	pcb->state_anterior = pcb->state;
	pcb->state = FINISH;
	log_info(log_kernel, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <%s>",pcb->contexto.PID,estado_pcb_a_string(pcb->state_anterior),estado_pcb_a_string(pcb->state));
	log_info(log_kernel, "Finaliza el proceso <%d> - Motivo: <%s>", pcb->contexto.PID,pcb->motivo_exit);
	send_FINALIZAR_ESTRUCTURAS(conexion_memoria);
	send_PID(conexion_memoria, pcb->contexto.PID);
	send_TABLA_SEGMENTOS(conexion_memoria,pcb->contexto.TSegmento);
	if(!send_PID(pcb->socket_consola, pcb->contexto.PID)){
		log_info(log_kernel, "Error enviandole a consola exit");
	};
	list_remove_element(lista_total_procesos, pcb);
	list_destroy_and_destroy_elements(pcb->tabla_archivosAbiertos, free);
	list_destroy_and_destroy_elements(pcb->recursos_asignados, free);
	list_destroy_and_destroy_elements(pcb->instrucciones, free);
	list_destroy_and_destroy_elements(pcb->contexto.TSegmento, free);

	free(pcb);
	sem_post(&multiprogramacion);
	pthread_mutex_unlock(&mutexExit);

}


//void remover_segmento(t_list* tabla_segmento, uint32_t id_segmento){
//	    bool encontrarSegmento(void* elemento) {
//	        segmento_t* segmento = (segmento_t*)elemento;
//	        return segmento->id == id_segmento;
//	    }
//
//	    segmento_t* segmentoEncontrado = list_remove_by_condition(tabla_segmento, encontrarSegmento);
//	    if (segmentoEncontrado != NULL) {
//	        // Liberar recursos asociados al segmento, si es necesario
//	        free(segmentoEncontrado);
//	    } else {
//	        log_error(log_kernel, "No se encontró el segmento con ID %d en la lista de segmentos", id_segmento);
//	    }
//
//}



