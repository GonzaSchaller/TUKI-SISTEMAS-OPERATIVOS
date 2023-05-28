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

	t_list* tseg;
	list_add(listaReady, pcb); //agrega a la listaReady

	log_info(log_kernel, "[READY] Entra el proceso de PID: %d a la cola.", pcb->PID);
	send_INICIAR_ESTRUCTURA_MEMORIA(conexion_memoria, "Inicializa las estructuras"); //mandamos mensaje a memoria que incie sus estructuras
	if(!recv_TABLA_SEGMENTOS(conexion_memoria, &tseg)){ //recibimos la direccion de la tabla de segmento  TODO agus memoria
		log_info(log_kernel, "No se recibio tabla de segmentos para el proceso de PID: %d", pcb->PID);
	}
	pcb->TSegmento = tseg; // TODO puntero?

	//printf("PROCESOS EN READY: %d \n", list_size(colaReady));
	log_debug(log_kernel,"[----------------PROCESOS EN READY: %d --------------------]\n", list_size(listaReady)); //max 4

	pthread_mutex_unlock(&mutexReady);
	sem_post(&contadorReady);
}

//estado Ready

//void agregarABlock(pcb_t* proceso){
//	//log_trace(log_kernel,"Entre en agregar a block ");
//	//log_info(log_kernel, "ADSSSS el proceso %d tiene el PC en %d",proceso->PID,proceso->PC);
//
//	pthread_mutex_lock(&mutexBlock);
//	list_add(listaBlock, proceso);
//	log_info(log_kernel, "[BLOCK] Entra el proceso de PID: %d a la cola.", proceso->PID);
//	pthread_mutex_unlock(&mutexBlock);
//	sem_post(&contadorBlock);
//}


//void sacarDeBlock(pcb_t* proceso){
//	sem_wait(&contadorBlock);
//	bool tienenMismoPID(void* elemento){
//
//				if(proceso->PID == ((pcb_t*) elemento)->PID) //se fija si un elemento x de la lista tiene el mismo PID que el proceso q queremos sacar
//					return true;
//				else
//					return false;
//			}
//
//	pthread_mutex_lock(&mutexBlock);
//	list_remove_by_condition(listaBlock, tienenMismoPID);
//	log_info(log_kernel, "[BLOCK] Sale el proceso de PID: %d de la cola.", proceso->PID);
//	pthread_mutex_unlock(&mutexBlock);
//}

void hiloNew_Ready(){

	while(1){
		sem_wait(&largoPlazo); //ver si sacarlo o no
		sem_wait(&multiprogramacion);
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

void bloquear_procesoPorIO(void* arg) {
    // Obtén el PCB del proceso a bloquear desde el argumento pasado al hilo
    pcb_t* pcb_bloqueado = (pcb_t*)arg;

    usleep(pcb_bloqueado->tiempo_bloqueo * 1000); //en microsegundos
    agregarAReady(pcb_bloqueado);

    pthread_exit(NULL);
}

recurso_sistema* encontrar_recurso(t_list* lista, char* nombre_buscar) {
    bool buscar_recurso(void* elemento) {
        recurso_sistema* recurso = (recurso_sistema*)elemento;
        return strcmp(recurso->nombre, nombre_buscar) == 0;
    }

    return (recurso_sistema*)list_find(lista, buscar_recurso);
}

void recalcular_rafagas_HRRN(pcb_t* pcb_siguiente, float tiempoDeFin){ // TODO cambiarle posicion
	uint32_t algoritmo_planif;

	algoritmo_planif = obtener_algoritmo_planificacion(algoritmo_planificacion);
	if(algoritmo_planif == HRRN){
				pcb_siguiente->rafaga_anterior_real =pcb_siguiente-> horaDeIngresoAExe - tiempoDeFin;//pcb_siguiente-> horaDeSalidaDeExe;
				pcb_siguiente->estimacion_prox_rafaga = (hrrn_alfa* pcb_siguiente->rafaga_anterior_real)+ ((1-hrrn_alfa)* pcb_siguiente-> estimacion_rafaga_anterior);
				pcb_siguiente->estimacion_rafaga_anterior = pcb_siguiente->estimacion_prox_rafaga;// } TODO ver a donde iria, si aca o en otro lado
				}
}

void manejar_recursos(pcb_t* pcb_siguiente, uint32_t cop, float tiempoDeFin){
	if (cop == WAIT)
				{
				    char* nombre_recurso;
				    if (recv_WAIT(conexion_cpu, &nombre_recurso))
				    {
						recurso_sistema* recurso = encontrar_recurso(lista_recursos, nombre_recurso);
				        if (recurso != NULL)
				        {
				            if (recurso->instancia > 0)
				                recurso->instancia--;
				            else
				            {
								recalcular_rafagas_HRRN(pcb_siguiente, tiempoDeFin);
				                // Bloquear el proceso actual en la cola de bloqueados del recurso
				                pthread_mutex_lock(&(recurso->mutexRecurso)); // creo que no es necesario el mutex, se comparte con otro hilo?
				                queue_push(recurso->colaBloqueados, pcb_siguiente);
				                pthread_mutex_unlock(&(recurso->mutexRecurso)); // Desbloquear el acceso a la cola de bloqueados
				            }
				        }
				        else { // si no existe el recurso
				        	terminarEjecucion(pcb_siguiente);
				        	sem_post(&multiprogramacion);
				        }
				    }
				    else log_error(log_kernel, "Fallo recibiendo WAIT");
				} //finaliza Wait

				else if (cop == SIGNAL)
				{
				    char* nombre_recurso;
				    if (recv_SIGNAL(conexion_cpu, &nombre_recurso))
				    {
				        recurso_sistema* recurso = encontrar_recurso(lista_recursos, nombre_recurso);
				        if (recurso != NULL)
				        {
				            recurso->instancia++;
				            if (queue_size(recurso->colaBloqueados) > 0)
				            {
				                pthread_mutex_lock(&(recurso->mutexRecurso));
				                queue_pop(recurso->colaBloqueados);
				                pthread_mutex_unlock(&(recurso->mutexRecurso));

				                pthread_mutex_lock(&mutexReady);
				                list_add(listaReady, pcb_siguiente);
				                pthread_mutex_unlock(&mutexReady);
				            }
				        } else{
							terminarEjecucion(pcb_siguiente);
							sem_post(&multiprogramacion);
						}
				    } else log_error(log_kernel, "Fallo recibiendo SIGNAL");
				}

}
void manejar_memoria(pcb_t* pcb_siguiente, uint32_t cop){
	uint32_t id_segmento,tamanio,base_nuevo_segmento,estado_segmento;
	segmento_t* segmento= NULL;
		if (cop == CREATE_SEGMENT) // id tamanio
		{
			if (recv_CREATE_SEGMENT(conexion_cpu, &id_segmento,&tamanio))
			{
				send_CREATE_SEGMENT(conexion_memoria,id_segmento,tamanio);

				if(recv(conexion_memoria, &estado_segmento, sizeof(estado_segmento), 0) == sizeof(estado_segmento))
				{
					if(estado_segmento == EXITOSO){

						if(recv_BASE_SEGMENTO(conexion_memoria,&base_nuevo_segmento))
						{	// crea exitosamente
							segmento->direccion_Base = base_nuevo_segmento;
							segmento->id = id_segmento;
							segmento->tamanio = tamanio;
							// mutex_wait (ejecucion de la lista)
							list_add(pcb_siguiente->TSegmento, segmento); // TODO ver si hay que poner un mutex
							// mutex_post ( ejecucion de la lista)
							send_BASE_SEGMENTO(conexion_cpu,base_nuevo_segmento);
						}
						else{
							log_error(log_kernel, "Fallo recibiendo BASE SEGMENTO");
						}
					}
					else if(estado_segmento == FALLIDO)
					{
						terminarEjecucion(pcb_siguiente);
						// TODO mostrar Out of Exit
						sem_post(&multiprogramacion);
					}
					else if(estado_segmento == COMPACTAR)
						{ // espacio disponible, pero no se encuentra contiguo, por lo que hay que compactar
						// TODO avisarle a agus memoria que nos mande COMPACTAR cuando se tiene el epacio pero no esta contiguo. Y fallido si no esta disponible
						//TODO compactar_memoria.
						}
				}
				else {
					log_error(log_kernel, "Fallo recibiendo CREATE_SEGMENT");// recibimos el espacio segmento
				}
			}// si recibimos de cpu lo que quiere que creemos

		}// si es create segment
		else if(cop == DELETE_SEGMENT)
		{
			//t_list* nueva_tabla_segmentos;
			if(recv_ID_SEGMENTO(conexion_cpu,&id_segmento)){
				send_ID_SEGMENTO(conexion_memoria, id_segmento);
			//recv_TABLA_SEGMENTOS(conexion_memoria,&nueva_tabla_segmentos); //
				//pcb_siguiente->TSegmento = *nueva_tabla_segmentos;// TODO ver si hay que eliminar el segmneto en la tabla
			}
			else log_error(log_kernel, "Fallo recibiendo DELETE_SEGMENT");
		}
		// devolver contexto de ejecucion a cpu para que continue con la ejecucion del proceso
}
void manejar_otras_instrucciones(pcb_t* pcb_siguiente,uint32_t cop, float tiempoDeFin){
	uint32_t tiempo_bloqueo_kernel;
	 if (cop == IO)
			{

		 	 	 if (!recv_tiempo_bloqueante(conexion_cpu, &tiempo_bloqueo_kernel)) { //tiempo que pasar en block
		 				log_error(log_kernel, "Fallo recibiendo el tiempo bloqueante");
		 			}
		 	 	 else if(pcb_siguiente->tiempo_bloqueo > 0){// caso bloqueo, agrega a ready cuando se termina de bloquear
					pthread_t hilo_Block;
					recalcular_rafagas_HRRN(pcb_siguiente, tiempoDeFin);
					//hilo porque quiero I/O en paralelo
					pthread_create(&hilo_Block, NULL, (void*)bloquear_procesoPorIO,(void*)pcb_siguiente);
					pthread_detach(hilo_Block);
					}
			}
	 else if(cop == YIELD){
		agregarAReady(pcb_siguiente);
	 }
	 else{// caso EXIT o error
					terminarEjecucion(pcb_siguiente);
					sem_post(&multiprogramacion); //le digo al new que ya puede mandar otro proceso mientras el grado de multiprog sea > 0
				}

}


void manejar_contextosDeEjecucion(pcb_t* pcb_siguiente){ // maneja lo que  nos manda cpu
	uint32_t cop,pc;
	if (!recv_PC(conexion_cpu, &pc)) { // TODO avisar cpu que nos manden primero program counter y despues el opcode y ver reg cpu
		log_error(log_kernel, "Fallo recibiendo pc");
			}
	pcb_siguiente->PC = pc;
	if(recv(conexion_cpu, &cop, sizeof(op_code), 0) == sizeof(op_code)) // Las que recibimos que SI son instruccion
			{
				time_t fin_exe = time(NULL);
				float tiempoDeFin = ((float) fin_exe); //TODO el 1000?

				manejar_recursos(pcb_siguiente, cop, tiempoDeFin);
				manejar_memoria(pcb_siguiente, cop);
				//manejar_fileSystem(pcb_siguiente, cop);
				manejar_otras_instrucciones(pcb_siguiente, cop, tiempoDeFin);

			}// del if del opcode
}// de la funcion

void hiloReady_Execute(){
	while(1)
	{
		pthread_mutex_lock(&multiprocesamiento);
		pcb_t* pcb_siguiente = obtener_siguiente_ready();

		enviar_pcb_cpu(conexion_cpu, pcb_siguiente); // lo estamos mandando a exe
		pcb_siguiente->horaDeIngresoAExe = ((float) time(NULL));
		// aca

		manejar_contextosDeEjecucion(pcb_siguiente);

		pthread_mutex_unlock(&multiprocesamiento);
	}
  }

void terminarEjecucion(pcb_t* pcb){ //TODO falta liberar recursos del proceso

	pthread_mutex_lock(&mutexExit);

	list_add(listaExit, pcb);
	log_info(log_kernel, "[EXIT] Finaliza el proceso de PID: %d", pcb->PID);

	pthread_mutex_unlock(&mutexExit);

}



