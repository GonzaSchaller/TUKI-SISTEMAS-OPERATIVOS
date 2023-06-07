#include "manejos.h"

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
void bloquear_procesoPorArchivo(void* argumentos) {
    // Obtén el PCB del proceso a bloquear desde el argumento pasado al hilo
    extra_code codigo;
    pcb_t* pcb_siguiente;
    arg_archivo_bloqueado* arg = (arg_archivo_bloqueado*)argumentos;
    pcb_t* pcb_bloqueado = arg->pcb;
    fcb_t* archivo = arg->archivo;
	int nombre_instruccion = arg->nombre_instruccion;
	free(arg);
    pcb_bloqueado->state_anterior = pcb_bloqueado->state;
    pcb_bloqueado->state = BLOCK;

    log_info(log_kernel,"PID: <%d> - Bloqueado por: <%s>",pcb_bloqueado->contexto.PID, archivo->nombreArchivo);

    if(nombre_instruccion == F_TRUNCATE){
		pthread_mutex_lock(&mutexHiloTruncate);
		recv_FINALIZAR_TRUNCATE(conexion_fileSystem,&codigo);
		if(codigo == FINALIZAR){
			pcb_siguiente =queue_pop(archivo->colaBloqueados);
		}
		pthread_mutex_unlock(&mutexHiloTruncate);
	}
    else if(nombre_instruccion == F_READ){
    	pthread_mutex_lock(&mutexHiloRead);
    	recv_FINALIZAR_READ(conexion_fileSystem, &codigo);
    	if(codigo == FINALIZAR){
			pcb_siguiente =queue_pop(archivo->colaBloqueados);
		}
        pthread_mutex_unlock(&mutexHiloRead);

    }
    else if(nombre_instruccion == F_WRITE){
    	pthread_mutex_lock(&mutexHiloWrite);
    	recv_FINALIZAR_WRITE(conexion_fileSystem,&codigo);
    	if(codigo == FINALIZAR){
			pcb_siguiente = queue_pop(archivo->colaBloqueados);

		}
        pthread_mutex_unlock(&mutexHiloWrite);
    }
	agregarAReady(pcb_siguiente);
    pthread_exit(NULL);
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
recurso_sistema* encontrar_recurso(t_list* lista, char* nombre_buscar) {
    bool buscar_recurso(void* elemento) {
        recurso_sistema* recurso = (recurso_sistema*)elemento;
        return strcmp(recurso->nombre, nombre_buscar) == 0;
    }

    return (recurso_sistema*)list_find(lista, buscar_recurso);
}
void manejar_memoria(pcb_t* pcb_siguiente, uint32_t cop){
	uint32_t id_segmento,tamanio,base_nuevo_segmento,estado_segmento;
	segmento_t* segmento= NULL;
	bool estado_while = true; // sigue adentro del while
		if (cop == CREATE_SEGMENT) // id tamanio
		{
			if (recv_CREATE_SEGMENT(conexion_cpu, &id_segmento,&tamanio))
			{
				while(estado_while) //while porque si hay que compactar tenemos que volver a mandarle create segment
				{
					send_CREATE_SEGMENT(conexion_memoria,id_segmento,tamanio);

					if(recv(conexion_memoria, &estado_segmento, sizeof(estado_segmento), 0) == sizeof(estado_segmento))
					{
						if(estado_segmento == EXITOSO)
						{

							if(recv_BASE_SEGMENTO(conexion_memoria,&base_nuevo_segmento))
							{	// crea exitosamente
								segmento->direccion_Base = base_nuevo_segmento;
								segmento->id = id_segmento;
								segmento->tamanio = tamanio;
								list_add(pcb_siguiente->contexto.TSegmento, segmento);
								send_BASE_SEGMENTO(conexion_cpu,base_nuevo_segmento);
								estado_while = false;
							}
							else log_error(log_kernel, "Fallo recibiendo BASE SEGMENTO");

						}
						else if(estado_segmento == FALLIDO)
						{
							terminarEjecucion(pcb_siguiente);

							log_info(log_kernel, "Finaliza el proceso <%d> - Motivo: <OUT OF MEMORY>", pcb_siguiente->contexto.PID);
							sem_post(&multiprogramacion);
							estado_while = false;
						}
						else if(estado_segmento == COMPACTAR)
						{ // espacio disponible, pero no se encuentra contiguo, por lo que hay que compactar
							sem_wait(&semFWrite);
							sem_wait(&semFRead);
							send(conexion_memoria, &estado_segmento, sizeof(estado_segmento), 0);// le mandamos COMPACTAR, porque no queriamos ahcer otro send
							recibir_actualizar_tablas_segmento(pcb_siguiente); //mandamos, recibimos y actualizamos las tabla de segmento
								//log_info(log_kernel, "Finaliza el proceso <%d> - Motivo: <SEG_FAULT>", pcb_proceso->contexto_PCB.PID); //todo log este

						}
						sem_post(&semFWrite);
						sem_post(&semFRead);
					}
					else log_error(log_kernel, "Fallo recibiendo CREATE_SEGMENT");// recibimos el espacio segmento

				}// si recibimos de cpu lo que quiere que creemos while
			}

		}// si es create segment
		else if(cop == DELETE_SEGMENT)
		{
			t_list* nueva_tabla_segmentos;
			if(recv_ID_SEGMENTO(conexion_cpu,&id_segmento)){
				send_ID_SEGMENTO(conexion_memoria, id_segmento);
				send_TABLA_SEGMENTOS(conexion_memoria, pcb_siguiente->contexto.TSegmento);//mandamos para que memoria la actualice
				recv_TABLA_SEGMENTOS(conexion_memoria,&nueva_tabla_segmentos); // cambie esto ahora nomas
				pcb_siguiente->contexto.TSegmento = nueva_tabla_segmentos;// cambie esto ahora nomas
			}
			else log_error(log_kernel, "Fallo recibiendo DELETE_SEGMENT");
		}

		send_CONTEXTO_EJECUCION(conexion_cpu, pcb_siguiente->contexto);// devolver contexto de ejecucion a cpu para que continue con la ejecucion del proceso
}
void manejar_otras_instrucciones(pcb_t* pcb_siguiente,uint32_t cop, float tiempoDeFin){
	float tiempo_bloqueo_kernel;
	 if (cop == IO)
			{

		 	 	 if (!recv_tiempo_bloqueante(conexion_cpu, &tiempo_bloqueo_kernel)) { //tiempo que pasar en block
		 				log_error(log_kernel, "Fallo recibiendo el tiempo bloqueante");
		 			}

		 	 	 else if(tiempo_bloqueo_kernel >= 0){// caso bloqueo, agrega a ready cuando se termina de bloquear
					pcb_siguiente->tiempo_bloqueo = tiempo_bloqueo_kernel;
					pthread_t hilo_Block;
					recalcular_rafagas_HRRN(pcb_siguiente, tiempoDeFin);
					log_info(log_kernel, "PID: <%d> - Ejecuta IO: <%f>", pcb_siguiente->contexto.PID, pcb_siguiente->tiempo_bloqueo);
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
					log_info(log_kernel, "Finaliza el proceso <%d> - Motivo: <SUCCESS> ", pcb_siguiente->contexto.PID);
					sem_post(&multiprogramacion); //le digo al new que ya puede mandar otro proceso mientras el grado de multiprog sea > 0
				}
	 else{
		 terminarEjecucion(pcb_siguiente);
		 log_error(log_kernel, "Finaliza por error");
	 }
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

							list_add(pcb_siguiente->tabla_archivosAbiertos,archivo->nombreArchivo); // agrego a la tabla de archivos abiertos del proceso

							queue_push(archivo->colaBloqueados, pcb_siguiente ); // cola de bloqueados del archivo, el pid del proceso que quiere usarlo
							pcb_siguiente->state_anterior = pcb_siguiente->state;
							pcb_siguiente->state = BLOCK;
							log_info(log_kernel, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <%s>",pcb_siguiente->contexto.PID,estado_pcb_a_string(pcb_siguiente->state_anterior),estado_pcb_a_string(pcb_siguiente->state));
							//pthread_mutex_unlock(&(archivo->mutexArchivo));

							recalcular_rafagas_HRRN(pcb_siguiente, tiempoDeFin);
				        }
				        else{
							if(!send_EXISTE_ARCHIVO(conexion_memoria, nombre_archivo)){
								log_error(log_kernel, "Fallo enviado existe_archivo a FILESYSTEM");
							}
								extra_code estado;
								if(!recv_OK_CODE(conexion_fileSystem, &estado)){
								log_error(log_kernel, "Fallo recibiendo OK de FILESYSTEM");
								}
								if(estado == INCORRECTO) {
									if(!send_CREAR_ARCHIVO(conexion_fileSystem, nombre_archivo, 0)){
										log_error(log_kernel, "Fallo enviado crear_archivo a FILESYSTEM");
									}
								}
								fcb_t* nuevo_archivo = malloc(sizeof(fcb_t));
								nuevo_archivo->colaBloqueados = queue_create();
								pthread_mutex_init(&(nuevo_archivo->mutexArchivo),NULL);
								nuevo_archivo->nombreArchivo = nombre_archivo;
								nuevo_archivo->tamanio_archivo =0;
								nuevo_archivo->puntero_directo =0;
								//nuevo_archivo->puntero_indirecto;// todo que iria aca?
								pthread_mutex_lock(&(nuevo_archivo->mutexArchivo));
								list_add(tabla_ArchivosAbiertosGlobal,nuevo_archivo); // TODO deberia ser el archivo nuevo, no el nombre del archivo nuevo solamente.

								list_add(pcb_siguiente->tabla_archivosAbiertos,nuevo_archivo->nombreArchivo); // agrego a la tabla de archivos abiertos del proceso
								send_CONTEXTO_EJECUCION(conexion_cpu, pcb_siguiente->contexto);
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
					// init del mutex
			    	//pthread_mutex_lock(&(archivo->mutexArchivo));
			    	uint32_t tamanioLista = queue_size(archivo->colaBloqueados);
					if(tamanioLista > 0){ // algun proceso quiere ese archivo
					pcb_t* pcb_bloqueado= queue_pop(archivo->colaBloqueados);
					agregarAReady(pcb_bloqueado);
					}
					else{
						// ningun otro proceso quiere el archivo
					eliminarArchivoDeLista(nombre_archivo, pcb_siguiente->tabla_archivosAbiertos); //YO no quiero mas tener este archivo abierto
					eliminarArchivoDeLista(nombre_archivo, tabla_ArchivosAbiertosGlobal);// sacar el archivo de la tabla global tambien			// ya NADIE lo tiene abierto
					//list_destroy_and_destroy_elements(archivo->colaBloqueados,free);
					//free(archivo);
					}
				}
				pthread_mutex_unlock(&(archivo->mutexArchivo)); //desbloquea el archivo para que otro proceso pueda usarlo
			}
		else log_error(log_kernel, "Fallo recibiendo FCLOSE");
	}

	else if (cop == F_SEEK){
		uint32_t posicion;
		char* nombre_archivo;
		if(recv_F_SEEK(conexion_cpu,&nombre_archivo,&posicion)){
			fcb_t* archivo = encontrar_archivo(tabla_ArchivosAbiertosGlobal,nombre_archivo);
			archivo->puntero_directo = posicion;
			send_CONTEXTO_EJECUCION(conexion_cpu, pcb_siguiente->contexto);
		}

	}
	else if (cop == F_TRUNCATE){
		uint32_t tamanio;
		char* nombre_archivo;
		if(recv_F_TRUNCATE(conexion_cpu,&nombre_archivo,&tamanio)){
			send_F_TRUNCATE(conexion_fileSystem,nombre_archivo,tamanio);
			fcb_t* archivo = encontrar_archivo(tabla_ArchivosAbiertosGlobal,nombre_archivo);
			queue_push(archivo->colaBloqueados, pcb_siguiente ); // cola de bloqueados del archivo, el pid del proceso que quiere usarlo
			log_info(log_kernel, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <%s>",pcb_siguiente->contexto.PID,estado_pcb_a_string(pcb_siguiente->state_anterior),estado_pcb_a_string(pcb_siguiente->state));
			arg_archivo_bloqueado* args = malloc(sizeof(arg_archivo_bloqueado));
			pthread_t hilo_archivo;
			args->pcb = pcb_siguiente;
			args->archivo = archivo;
			args->nombre_instruccion = F_TRUNCATE;
			pthread_create(&hilo_archivo, NULL, (void*)bloquear_procesoPorArchivo,(void*)args);
			pthread_detach(hilo_archivo);
			recalcular_rafagas_HRRN(pcb_siguiente, tiempoDeFin);

		}
	}
	else if(cop ==F_READ){
		sem_wait(&semFRead);
		char* nombre_archivo;
		uint32_t direccion_logica;
		uint32_t cantidad_bytes;

		if(!recv_F_READ(conexion_cpu, &nombre_archivo, &direccion_logica, &cantidad_bytes)){
			log_error(log_kernel, "Fallo recibiendo F_READ");
		}
		else {
			if(!send_F_READ(conexion_fileSystem, nombre_archivo, direccion_logica, cantidad_bytes)){
				log_error(log_kernel, "Fallo enviando F_READ a FS");
			}
			fcb_t* archivo = encontrar_archivo(tabla_ArchivosAbiertosGlobal,nombre_archivo);
			arg_archivo_bloqueado* args = malloc(sizeof(arg_archivo_bloqueado));
			pthread_t hilo_archivo;
			args->pcb = pcb_siguiente;
			args->archivo = archivo;
			args->nombre_instruccion = F_READ;
			pthread_create(&hilo_archivo, NULL, (void*)bloquear_procesoPorArchivo,(void*)args);
			pthread_detach(hilo_archivo);
			recalcular_rafagas_HRRN(pcb_siguiente, tiempoDeFin);
		}
		sem_post(&semFRead);
	}


	else if(cop == F_WRITE){
		sem_wait(&semFWrite);
		char* nombre_archivo;
		uint32_t direccion_logica;
		uint32_t cantidad_bytes;

		if(!recv_F_WRITE(conexion_cpu, &nombre_archivo, &direccion_logica, &cantidad_bytes)){
			log_error(log_kernel, "Fallo recibiendo F_WRITE");
		}
		else{
			if(!send_F_WRITE(conexion_fileSystem, nombre_archivo, direccion_logica, cantidad_bytes)){
				log_error(log_kernel, "Fallo enviando F_READ a FS");
			}
			fcb_t* archivo = encontrar_archivo(tabla_ArchivosAbiertosGlobal,nombre_archivo);
			arg_archivo_bloqueado* args = malloc(sizeof(arg_archivo_bloqueado));
			pthread_t hilo_archivo;
			args->pcb = pcb_siguiente;
			args->archivo = archivo;
			args->nombre_instruccion = F_WRITE;
			pthread_create(&hilo_archivo, NULL, (void*)bloquear_procesoPorArchivo,(void*)args);
			pthread_detach(hilo_archivo);
			recalcular_rafagas_HRRN(pcb_siguiente, tiempoDeFin);
		}
		sem_post(&semFWrite);
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
void manejar_contextosDeEjecucion(pcb_t* pcb_siguiente){ // maneja lo que  nos manda cpu
	uint32_t cop;
	contexto_ejecucion contexto;


	if(recv(conexion_cpu, &cop, sizeof(op_code), 0) == sizeof(op_code)) // Las que recibimos que SI son instruccion
			{
				if (!recv_CONTEXTO_EJECUCION(conexion_cpu, &contexto))
				{
				log_error(log_kernel, "Fallo recibiendo el Contexto de Ejecucion");
				}

				else{
				pcb_siguiente->contexto= contexto;
				time_t fin_exe = time(NULL);
				float tiempoDeFin = ((float) fin_exe) * 1000;
				//aca
				manejar_recursos(pcb_siguiente, cop, tiempoDeFin);
				manejar_memoria(pcb_siguiente, cop);
				manejar_fileSystem(pcb_siguiente, cop, tiempoDeFin);
				manejar_otras_instrucciones(pcb_siguiente, cop, tiempoDeFin);
				}

			}// del if del opcode

}// de la funcion
