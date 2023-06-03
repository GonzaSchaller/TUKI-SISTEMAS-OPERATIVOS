#include "planificador.h"


t_queue* colaNew;
t_list* listaReady;
t_list* listaExe;
t_list* listaBlock;
t_list* listaExit;
t_list* tabla_ArchivosAbiertosGlobal;

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

	time_t a = time(NULL); //momento en el que entra un proceso, sirve para el HRRN
	pcb->horaDeIngresoAReady = a;
	pthread_mutex_lock(&mutexReady); //para el execute a ready y de blocked a ready y de new a ready

	t_list* tseg;
	list_add(listaReady, pcb); //agrega a la listaReady

	send_INICIAR_ESTRUCTURA_MEMORIA(conexion_memoria); //mandamos mensaje a memoria que incie sus estructuras

	if(!recv_TABLA_SEGMENTOS(conexion_memoria, &tseg)){ //recibimos la direccion de la tabla de segmento
		log_info(log_kernel, "No se recibio tabla de segmentos para el proceso de PID: %d", pcb->contexto.PID);
	}
	pcb->contexto.TSegmento = tseg;
	pcb->state_anterior = pcb->state;
	pcb->state = READY;
	log_info(log_kernel, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <%s>",pcb->contexto.PID,estado_pcb_a_string(pcb->state_anterior),estado_pcb_a_string(pcb->state));
	print_lista_PID();
	pthread_mutex_unlock(&mutexReady);
	sem_post(&contadorReady);
}

void agregarAReady(pcb_t* pcb){
	time_t llegadaReady = time(NULL);
	pcb->horaDeIngresoAReady = llegadaReady;
	pthread_mutex_lock(&mutexReady);
	list_add(listaReady, pcb);
	pcb->state_anterior = pcb->state;
	pcb->state = READY;
	log_info(log_kernel, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <%s>",pcb->contexto.PID,estado_pcb_a_string(pcb->state_anterior),estado_pcb_a_string(pcb->state));
	print_lista_PID();
	pthread_mutex_unlock(&mutexReady);

}

void hiloNew_Ready(){

	while(1){
		sem_wait(&largoPlazo); //ver si sacarlo o no
		sem_wait(&multiprogramacion);
		pcb_t* proceso = sacarDeNew();
		//log_error(log_kernel,"[===========] agregue a ready desde hilo new ready");
		agregarNewAReady(proceso);

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
    // aca
    pcb_bloqueado->state_anterior = pcb_bloqueado->state;
    pcb_bloqueado->state = BLOCK;
    log_info(log_kernel,"PID: <%d> - Bloqueado por: <IO>",pcb_bloqueado->contexto.PID);
    usleep(pcb_bloqueado->tiempo_bloqueo * 1000); //en microsegundos
    agregarAReady(pcb_bloqueado);
    //pcb_bloqueado->state = READY;

    pthread_exit(NULL);
}

recurso_sistema* encontrar_recurso(t_list* lista, char* nombre_buscar) {
    bool buscar_recurso(void* elemento) {
        recurso_sistema* recurso = (recurso_sistema*)elemento;
        return strcmp(recurso->nombre, nombre_buscar) == 0;
    }

    return (recurso_sistema*)list_find(lista, buscar_recurso);
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

void manejar_recursos(pcb_t* pcb_siguiente, uint32_t cop, float tiempoDeFin){
	if (cop == WAIT)
				{
				    char* nombre_recurso;
				    if (recv_WAIT(conexion_cpu, &nombre_recurso))
				    {
						recurso_sistema* recurso = encontrar_recurso(lista_recursos, nombre_recurso);
				        if (recurso != NULL)
				        {
				            if (recurso->instancia > 0){
				                recurso->instancia--;
				                if(!encontrar_recurso(pcb_siguiente->recursos_asignados, nombre_recurso)) //si no encontro el recurso, agrega el recurso a la lista del proceso
				                asignar_recurso(nombre_recurso , pcb_siguiente->recursos_asignados);
				                else
				                aumentar_instancias_recurso(nombre_recurso , pcb_siguiente->recursos_asignados); // si ya tiene el recurso asignado, le sumo una instancia

				            }
				            else
				            {
								recalcular_rafagas_HRRN(pcb_siguiente, tiempoDeFin);
				                // Bloquear el proceso actual en la cola de bloqueados del recurso
				                pthread_mutex_lock(&(recurso->mutexRecurso)); // creo que no es necesario el mutex, se comparte con otro hilo?
				                queue_push(recurso->colaBloqueados, pcb_siguiente);
				                pcb_siguiente->state_anterior = pcb_siguiente->state;
				                pcb_siguiente->state = BLOCK;
				                log_info(log_kernel,"PID: <%d> - Bloqueado por: <%s>",pcb_siguiente->contexto.PID,recurso->nombre);
				                log_info(log_kernel, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <%s>",pcb_siguiente->contexto.PID,estado_pcb_a_string(pcb_siguiente->state_anterior),estado_pcb_a_string(pcb_siguiente->state));
				                pthread_mutex_unlock(&(recurso->mutexRecurso)); // Desbloquear el acceso a la cola de bloqueados
				            }
				            log_info(log_kernel,"PID: <%d> - Wait: < %s > - Instancias: <%d>",pcb_siguiente->contexto.PID, recurso->nombre, recurso->instancia);
				        }
				        else { // si no existe el recurso
				        	terminarEjecucion(pcb_siguiente);
							log_info(log_kernel, "Finaliza el proceso <%d> porque no existe una instancia del recurso", pcb_siguiente->contexto.PID);
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
				                pcb_t* pcb_bloqueado = queue_pop(recurso->colaBloqueados);
				                pthread_mutex_unlock(&(recurso->mutexRecurso));
				               	agregarAReady(pcb_bloqueado);
				            }
				            log_info(log_kernel,"PID: <%d> - Signal: <%s> - Instancias: <%d>",pcb_siguiente->contexto.PID, recurso->nombre, recurso->instancia);
				        } else{
							terminarEjecucion(pcb_siguiente);
							log_info(log_kernel, "Finaliza el proceso <%d> porque no existe una instancia del recurso", pcb_siguiente->contexto.PID);
							sem_post(&multiprogramacion);
						}
				    } else  log_error(log_kernel, "Fallo recibiendo SIGNAL");
				}

}
void remover_segmento(t_list* tabla_segmento, uint32_t id_segmento){
	    bool encontrarSegmento(void* elemento) {
	        segmento_t* segmento = (segmento_t*)elemento;
	        return segmento->id == id_segmento;
	    }

	    segmento_t* segmentoEncontrado = list_remove_by_condition(tabla_segmento, encontrarSegmento);
	    if (segmentoEncontrado != NULL) {
	        // Liberar recursos asociados al segmento, si es necesario
	        free(segmentoEncontrado);
	    } else {
	        log_error(log_kernel, "No se encontró el segmento con ID %d en la lista de segmentos", id_segmento);
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
							list_add(pcb_siguiente->contexto.TSegmento, segmento); // TODO ver si hay que poner un mutex
							// mutex_post ( ejecucion de la lista)
							send_BASE_SEGMENTO(conexion_cpu,base_nuevo_segmento);
						}
						else{
							log_error(log_kernel, "Fallo recibiendo BASE SEGMENTO");
						}
					// todo crear segmento log
					}
					else if(estado_segmento == FALLIDO)
					{
						terminarEjecucion(pcb_siguiente);

						log_info(log_kernel, "Finaliza el proceso <%d> - Motivo: <OUT OF MEMORY>", pcb_siguiente->contexto.PID); // todo chat gpt neus
						sem_post(&multiprogramacion);
					}
					else if(estado_segmento == COMPACTAR)
						{ // espacio disponible, pero no se encuentra contiguo, por lo que hay que compactar
						// TODO avisarle a agus memoria que nos mande COMPACTAR cuando se tiene el epacio pero no esta contiguo. Y fallido si no esta disponible
						//TODO compactar_memoria.
						//log_info(log_kernel, "Finaliza el proceso <%d> - Motivo: <SEG_FAULT>", pcb_proceso->contexto_PCB.PID);
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
				remover_segmento(pcb_siguiente ->contexto.TSegmento, id_segmento); //TODO hay que esperar y mandar la tabla de segmentos para que actualice memoria
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
					log_info(log_kernel, "PID: <%d> - Ejecuta IO: <%f>", pcb_siguiente->contexto.PID, tiempoDeFin); // todo ver tiempo * 1000? Me parece que tendriamos que hacerlo por la IO
					//hilo porque quiero I/O en paralelo

					pthread_create(&hilo_Block, NULL, (void*)bloquear_procesoPorIO,(void*)pcb_siguiente);
					pthread_detach(hilo_Block);
					}
			}
	 else if(cop == YIELD){
		recalcular_rafagas_HRRN(pcb_siguiente, tiempoDeFin);
		agregarAReady(pcb_siguiente);

	 }
	 else if(cop == EXIT){// caso EXIT
					terminarEjecucion(pcb_siguiente);
					log_info(log_kernel, "Finaliza el proceso <%d> - Motivo: <SUCCESS> ", pcb_siguiente->contexto.PID); // todo poner el success bien
					sem_post(&multiprogramacion); //le digo al new que ya puede mandar otro proceso mientras el grado de multiprog sea > 0
				}
	 else{
		 terminarEjecucion(pcb_siguiente);
		 log_error(log_kernel, "Finaliza por error");
	 }
}


void manejar_contextosDeEjecucion(pcb_t* pcb_siguiente){ // maneja lo que  nos manda cpu
	uint32_t cop;
	contexto_ejecucion contexto;
	if (!recv_CONTEXTO_EJECUCION(conexion_cpu, &contexto)) {
		log_error(log_kernel, "Fallo recibiendo el Contexto de Ejecucion");
			}
	pcb_siguiente->contexto= contexto;
	if(recv(conexion_cpu, &cop, sizeof(op_code), 0) == sizeof(op_code)) // Las que recibimos que SI son instruccion
			{
				time_t fin_exe = time(NULL);
				float tiempoDeFin = ((float) fin_exe); //TODO el 1000?

				manejar_recursos(pcb_siguiente, cop, tiempoDeFin);
				manejar_memoria(pcb_siguiente, cop);
				manejar_fileSystem(pcb_siguiente, cop, tiempoDeFin);
				manejar_otras_instrucciones(pcb_siguiente, cop, tiempoDeFin);

			}// del if del opcode
}// de la funcion

void hiloReady_Execute(){
	while(1)
	{
		pthread_mutex_lock(&multiprocesamiento);
		pcb_t* pcb_siguiente = obtener_siguiente_ready();

		log_info(log_kernel, "estamos en execute %d", pcb_siguiente->contexto.PID); // log de prueba
		enviar_pcb_cpu(conexion_cpu, pcb_siguiente); // lo estamos mandando a exe
		pcb_siguiente->state_anterior = pcb_siguiente->state;
		pcb_siguiente->state = EXEC;
		log_info(log_kernel, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <%s>",pcb_siguiente->contexto.PID,estado_pcb_a_string(pcb_siguiente->state_anterior),estado_pcb_a_string(pcb_siguiente->state));
		pcb_siguiente->horaDeIngresoAExe = ((float) time(NULL));


		while(pcb_siguiente->state == EXEC){ //que ejecute cada instruccion hasta que cambie de estado
			manejar_contextosDeEjecucion(pcb_siguiente);

		 }
		pthread_mutex_unlock(&multiprocesamiento);

	}
  }

void liberar_Recursos(pcb_t* pcb) {
    for (int i = 0; i < list_size(pcb->recursos_asignados); i++) {
        recurso_sistema* recurso = list_get(pcb->recursos_asignados, i);
        recurso_sistema* recursoPosta = encontrar_recurso(lista_recursos, recurso->nombre);
        recursoPosta->instancia += recurso->instancia;
	}

}
void terminarEjecucion(pcb_t* pcb){ //TODO falta liberar recursos del proceso //free(pcb)? o liberar recursos tipo wait //ademas creo que hay que mandar mensaje a nmemoria para que libere sus recursos

	pthread_mutex_lock(&mutexExit);
	liberar_Recursos(pcb);
	list_add(listaExit, pcb);
	pcb->state_anterior = pcb->state;
	pcb->state = FINISH;
	log_info(log_kernel, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <%s>",pcb->contexto.PID,estado_pcb_a_string(pcb->state_anterior),estado_pcb_a_string(pcb->state)); // TODO dudoso q vaya aca
	send_FINALIZAR_ESTRUCTURAS(conexion_memoria);
	log_info(log_kernel, "[EXIT] Finaliza el proceso de PID: %d", pcb->contexto.PID);
	if(!send_EXIT(pcb->socket_consola)){
		log_info(log_kernel, "Error enviandole a consola exit");
	};
	free(pcb);
	pthread_mutex_unlock(&mutexExit);

}

void eliminarArchivoDeLista(char* nombreArchivo, t_list* listaArchivos) {
    // Buscar el archivo en la lista por nombre
	 bool encontrarArchivo(void* elemento) {
	        fcb_t* archivo = (fcb_t*)elemento;
	        return strcmp(archivo->nombreArchivo, nombreArchivo) == 0;
	    }

    void* archivoEncontrado = list_remove_by_condition(listaArchivos, encontrarArchivo);

    if (archivoEncontrado != NULL) {
    	fcb_t* archivo = (fcb_t*)archivoEncontrado;
    	free(archivo);
    }
}
fcb_t* encontrar_archivo(t_list* lista_archivos, char* nombre_archivo) {
    // Función de condición para buscar el archivo por nombre
    bool encontrar_archivo(void* elemento) {
        fcb_t* archivo = (fcb_t*)elemento;
        return strcmp(archivo->nombreArchivo, nombre_archivo) == 0;
    }

    // Buscar el archivo en la lista
    fcb_t* archivo_encontrado = (fcb_t*)list_find(lista_archivos, encontrar_archivo);

    return archivo_encontrado;
}

void manejar_fileSystem(pcb_t* pcb_siguiente, uint32_t cop, float tiempoDeFin){
	if (cop == F_OPEN) // es un wait
				{
				    char* nombre_archivo;
				    if (recv_F_OPEN(conexion_cpu, &nombre_archivo))
				    {
				        fcb_t* archivo = encontrar_archivo(tabla_ArchivosAbiertosGlobal,nombre_archivo);
				        if (archivo != NULL)
				        {// se agrega la entrada en la tabla de archivos abietos del proceso con el puntero en 0


				        	archivo->puntero_directo = 0;

							list_add(pcb_siguiente->tabla_archivosAbiertos,archivo); // agrego a la tabla de archivos abiertos del proceso

							pthread_mutex_init(&archivo->mutexArchivo, NULL); // TODO queda raro aca
							pthread_mutex_lock(&(archivo->mutexArchivo));
							queue_push(archivo->colaBloqueados, pcb_siguiente ); // cola de bloqueados del archivo, el pid del proceso que quiere usarlo
							pcb_siguiente->state_anterior = pcb_siguiente->state;
							pcb_siguiente->state = BLOCK;
							log_info(log_kernel, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <%s>",pcb_siguiente->contexto.PID,estado_pcb_a_string(pcb_siguiente->state_anterior),estado_pcb_a_string(pcb_siguiente->state));
							pthread_mutex_unlock(&(archivo->mutexArchivo));

							recalcular_rafagas_HRRN(pcb_siguiente, tiempoDeFin);
				        }
				        else{
							//preguntar al filesystem si el archivo existe //todo
							// send_EXISTE_ARCHIVO(conexion_memoria,nombre_archivo); // TODO

//							if(NO EXISTE) {
//							send filesystem crear archivo tamanio 0
//							}
//							agregar entrada a la tabla global de archivos abiertos y a la del proceso con el puntero en 0
//							devolver a cpu el contexto de ejecuiciuon para que continue
//							// lo otro
//
						}
				    }
				    else log_error(log_kernel, "Fallo recibiendo FOPEN");
				}
	else if(cop == F_CLOSE)
	{
		char* nombre_archivo;

		if (recv_F_CLOSE(conexion_cpu, &nombre_archivo))
			{
			    fcb_t* archivo = encontrar_archivo(tabla_ArchivosAbiertosGlobal,nombre_archivo);
			    if (archivo != NULL)
			    {
			    	pthread_mutex_lock(&(archivo->mutexArchivo));
			    	uint32_t tamanioLista = queue_size(archivo->colaBloqueados);

					if(tamanioLista > 0){ // algun proceso quiere ese archivo
					pcb_t* pcb_bloqueado= queue_pop(archivo->colaBloqueados);
					pthread_mutex_unlock(&(archivo->mutexArchivo));
					agregarAReady(pcb_bloqueado);

					}
					else{
						// ningun otro proceso quiere el archivo
					eliminarArchivoDeLista(nombre_archivo, pcb_siguiente->tabla_archivosAbiertos); //YO no quiero mas tener este archivo abierto
					eliminarArchivoDeLista(nombre_archivo, tabla_ArchivosAbiertosGlobal);// sacar el archivo de la tabla global tambien			// ya NADIE lo tiene abierto
					}
				}
			}
		else log_error(log_kernel, "Fallo recibiendo FCLOSE");
	}

	else if (cop == F_SEEK){
		uint32_t posicion;
		char* nombre_archivo;
		if(recv_F_SEEK(conexion_cpu,&nombre_archivo,&posicion)){
			fcb_t* archivo = encontrar_archivo(tabla_ArchivosAbiertosGlobal,nombre_archivo); // todo ver si hay una forma de generalizarla, no repetirla tantas veces
				 archivo->puntero_directo = posicion;
				// send contexto de ejecucion; TODO devolvemos un mensaje de confirmacion

		}
	}
	else if (cop == F_TRUNCATE){
		uint32_t tamanio;
		char* nombre_archivo;
		if(recv_F_TRUNCATE(conexion_cpu,&nombre_archivo,&tamanio)){
			send_F_TRUNCATE(conexion_fileSystem,nombre_archivo,tamanio);
			fcb_t* archivo = encontrar_archivo(tabla_ArchivosAbiertosGlobal,nombre_archivo);
			//todo bloqueará al proceso hasta que el File System informe de la finalización de la operación.
			pthread_mutex_init(&archivo->mutexArchivo, NULL); // TODO queda raro aca
			pthread_mutex_lock(&(archivo->mutexArchivo));
			queue_push(archivo->colaBloqueados, pcb_siguiente ); // cola de bloqueados del archivo, el pid del proceso que quiere usarlo
			pcb_siguiente->state_anterior = pcb_siguiente->state;
			pcb_siguiente->state = BLOCK;
			log_info(log_kernel, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <%s>",pcb_siguiente->contexto.PID,estado_pcb_a_string(pcb_siguiente->state_anterior),estado_pcb_a_string(pcb_siguiente->state));
			pthread_mutex_unlock(&(archivo->mutexArchivo));

			recalcular_rafagas_HRRN(pcb_siguiente, tiempoDeFin); // todo generalizar edsde el mutex init hasta aca
			// todo falta desbloquear

		}
	}
	else if(cop ==F_READ){
		//todo
	}


	else if(cop == F_WRITE){
			//todo
	}
}





