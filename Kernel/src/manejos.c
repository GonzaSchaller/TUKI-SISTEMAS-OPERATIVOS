#include "manejos.h"
bool noSalePorIO;

void bloquear_procesoPorIO(void* arg) {
    // Obtén el PCB del proceso a bloquear desde el argumento pasado al hilo
    pcb_t* pcb_bloqueado = (pcb_t*)arg;
    // aca
    pcb_bloqueado->state_anterior = pcb_bloqueado->state;
    pcb_bloqueado->state = BLOCK;
    log_info(log_kernel,"PID: <%d> - Bloqueado por: <IO>",pcb_bloqueado->contexto.PID);
    usleep(pcb_bloqueado->tiempo_bloqueo * 1000000); //en microsegundos
    agregarAReady(pcb_bloqueado);
    //pcb_bloqueado->state = READY;

    pthread_exit(NULL);
}
void bloquear_procesoPorArchivo(void* argumentos) {
    // Obtén el PCB del proceso a bloquear desde el argumento pasado al hilo
    extra_code codigo;
    arg_archivo_bloqueado* arg = (arg_archivo_bloqueado*)argumentos;
    pcb_t* pcb_bloqueado = arg->pcb;
    fcb_kernel* archivo = arg->archivo;
	int nombre_instruccion = arg->nombre_instruccion;
	free(arg);
	log_info(log_kernel, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <%s>",pcb_bloqueado->contexto.PID,estado_pcb_a_string(pcb_bloqueado->state_anterior),estado_pcb_a_string(pcb_bloqueado->state));
    log_info(log_kernel,"PID: <%d> - Bloqueado por: <%s>",pcb_bloqueado->contexto.PID, archivo->nombreArchivo);

    if(nombre_instruccion == F_TRUNCATE){
		pthread_mutex_lock(&mutexHiloTruncate);
		recv_OK_CODE(conexion_fileSystem,&codigo);// ok code?
		if(codigo == FINALIZAR){
			pcb_bloqueado =list_remove(archivo->colaBloqueados,list_size(archivo->colaBloqueados)-1);
		}
		pthread_mutex_unlock(&mutexHiloTruncate);
	}
    else if(nombre_instruccion == F_READ){
    	pthread_mutex_lock(&mutexHiloRead);
    	recv_OK_CODE(conexion_fileSystem, &codigo);
    	if(codigo == FINALIZAR){
			pcb_bloqueado = list_remove(archivo->colaBloqueados,list_size(archivo->colaBloqueados)-1);
			sem_post(&semFRead);
		}
        pthread_mutex_unlock(&mutexHiloRead);

    }
    else if(nombre_instruccion == F_WRITE){
    	pthread_mutex_lock(&mutexHiloWrite);
    	recv_OK_CODE(conexion_fileSystem,&codigo);
    	if(codigo == FINALIZAR){
			pcb_bloqueado = list_remove(archivo->colaBloqueados,list_size(archivo->colaBloqueados)-1);
			sem_post(&semFWrite);
		}
        pthread_mutex_unlock(&mutexHiloWrite);
    }
	agregarAReady(pcb_bloqueado);
    pthread_exit(NULL);
}
void eliminarArchivoDeTablaGlobal(char* nombreArchivo, t_list* listaArchivos) {
    // Buscar el archivo en la lista por nombre
	 bool encontrarArchivo(void* elemento) {
		 fcb_kernel* archivo = (fcb_kernel*)elemento;
	        return strcmp(archivo->nombreArchivo, nombreArchivo) == 0;
	    }

    void* archivoEncontrado = list_remove_by_condition(listaArchivos, encontrarArchivo);

    if (archivoEncontrado != NULL) {
    	fcb_kernel* archivo = (fcb_kernel*)archivoEncontrado;
    	free(archivo->colaBloqueados);
    	pthread_mutex_destroy(&archivo->mutexArchivo);
    	free(archivo);

    }
}
void eliminarArchivoDeTablaProceso(char* nombreArchivo, t_list* listaArchivos) {
    // Buscar el archivo en la lista por nombre
	 bool encontrarArchivo(void* elemento) {
		 fcb_por_proceso* archivo = (fcb_por_proceso*)elemento;
	        return strcmp(archivo->nombreArchivo, nombreArchivo) == 0;
	    }

    void* archivoEncontrado = list_remove_by_condition(listaArchivos, encontrarArchivo);

    if (archivoEncontrado != NULL) {
    	fcb_por_proceso* archivo = (fcb_por_proceso*)archivoEncontrado;
    	free(archivo);
    }
}
fcb_kernel* encontrar_archivoTablaGlobal(t_list* lista_archivos, char* nombre_archivo) {
    // Función de condición para buscar el archivo por nombre
    bool encontrar_archivo(void* elemento) {
        fcb_kernel* archivo = (fcb_kernel*)elemento;
        return strcmp(archivo->nombreArchivo, nombre_archivo) == 0;
    }

    // Buscar el archivo en la lista
    fcb_kernel* archivo_encontrado = (fcb_kernel*)list_find(lista_archivos, encontrar_archivo);

    return archivo_encontrado;
}
fcb_por_proceso* encontrar_archivoTablaProceso(t_list* lista_archivos, char* nombre_archivo) {
    // Función de condición para buscar el archivo por nombre
    bool encontrar_archivo(void* elemento) {
    	fcb_por_proceso* archivo = (fcb_por_proceso*)elemento;
        return strcmp(archivo->nombreArchivo, nombre_archivo) == 0;
    }

    // Buscar el archivo en la lista
    fcb_por_proceso* archivo_encontrado = (fcb_por_proceso*)list_find(lista_archivos, encontrar_archivo);

    return archivo_encontrado;
}
recurso_sistema* encontrar_recurso(t_list* lista, char* nombre_buscar) {
    int t = list_size(lista);
    for (int i = 0; i < t; i++) {
        recurso_sistema* recurso = (recurso_sistema*)list_get(lista, i);
        if (strcmp(recurso->nombre, nombre_buscar) == 0) {
            return recurso;
        }
    }
    return NULL;  // Si no se encuentra el recurso, se devuelve NULL
}

void recibir_actualizar_tablas_segmento(pcb_t* pcb_actual){
	pcb_t* proceso_planificado= NULL;
	t_list* tabla_nueva = NULL;
	segmento_t* segmento = NULL;

	pthread_mutex_lock(&mutexReady);
	int t = list_size(lista_total_procesos)-1;

	for (int i = 0; i < t + 1; i++) {
	    bool buscandoPID = true;
	    recv_TABLA_SEGMENTOS(conexion_memoria, &tabla_nueva);

	    segmento = list_get(tabla_nueva, 0);

	    int j = 0;
	    while (buscandoPID && j < list_size(lista_total_procesos)) {
	        proceso_planificado = list_get(lista_total_procesos, j);
	        if (proceso_planificado->contexto.PID == segmento->pid) {
	            proceso_planificado->contexto.TSegmento = tabla_nueva;
	            buscandoPID = false;
	        }
	        j++;
	    }
	}
	 	 pthread_mutex_unlock(&mutexReady);
}

void manejar_memoria(pcb_t* pcb_siguiente, uint32_t cop,uint32_t* seguir_ejecutando){
	uint32_t id_segmento,tamanio,base_nuevo_segmento,estado_segmento;
	segmento_t* segmento= malloc(sizeof(segmento_t));
	bool se_compacto=false;
	bool estado_while = true; // sigue adentro del while/
		if (cop == CREATE_SEGMENT) // id tamanio
		{
			if (recv_CREATE_SEGMENT(conexion_cpu, &id_segmento,&tamanio))
			{
				while(estado_while) //while porque si hay que compactar tenemos que volver a mandarle create segment
				{
					send_CREATE_SEGMENT(conexion_memoria,id_segmento,tamanio);
					send_PID(conexion_memoria, pcb_siguiente->contexto.PID);
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
								estado_while = false;
								send_seguir_ejecutando(conexion_cpu,1); // todo lo cambio a 1 (revisar)
								enviar_pcb_cpu(conexion_cpu, pcb_siguiente);
								//send_CONTEXTO_EJECUCION(conexion_cpu, pcb_siguiente->contexto);
								if(se_compacto){
								log_info(log_kernel, "Compactación: <Se finalizó el proceso de compactación>");
								sem_post(&semFWrite);
								sem_post(&semFRead);
								}
								log_info(log_kernel, "PID: <%d> - Crear Segmento - Id: <%d> - Tamaño: <%d>",pcb_siguiente->contexto.PID,id_segmento,tamanio);

							}
							else log_error(log_kernel, "Fallo recibiendo BASE SEGMENTO");

						}
						else if(estado_segmento == FALLIDO)
						{
                            pcb_siguiente->motivo_exit = "OUT_OF_MEMORY";
							send_seguir_ejecutando(conexion_cpu,1);
                            pcb_siguiente->finalizar_proceso = true;
							estado_while = false;
						}
						else if(estado_segmento == COMPACTAR)
						{ // espacio disponible, pero no se encuentra contiguo, por lo que hay que compactar
							log_info(log_kernel, "Esperando Fin de Operaciones de FS");
							sem_wait(&semFWrite);
							sem_wait(&semFRead);
							send(conexion_memoria, &estado_segmento, sizeof(estado_segmento), 0);// le mandamos COMPACTAR, porque no queriamos ahcer otro send
							log_info(log_kernel, "Compactación: <Se solicitó compactación>");
							recibir_actualizar_tablas_segmento(pcb_siguiente); //mandamos, recibimos y actualizamos las tabla de segmento
							se_compacto=true;
						}
						// devolver contexto de ejecucion a cpu para que continue con la ejecucion del proceso
					}
					else log_error(log_kernel, "Fallo recibiendo CREATE_SEGMENT");// recibimos el espacio segmento
				}// si recibimos de cpu lo que quiere que creemos while
			}

		}// si es create segment
		else if(cop == DELETE_SEGMENT)
		{
			t_list* nueva_tabla_segmentos;
			if(recv_ID_SEGMENTO(conexion_cpu,&id_segmento)){
				send_DELETE_SEGMENT(conexion_memoria, id_segmento);
				send_TABLA_SEGMENTOS(conexion_memoria, pcb_siguiente->contexto.TSegmento);//mandamos para que memoria la actualice
				send_PID(conexion_memoria,pcb_siguiente->contexto.PID);
				recv_TABLA_SEGMENTOS(conexion_memoria,&nueva_tabla_segmentos); // cambie esto ahora nomas

				pcb_siguiente->contexto.TSegmento = nueva_tabla_segmentos;// cambie esto ahora nomas
				send_seguir_ejecutando(conexion_cpu,1); //todo cambie esto
			    enviar_pcb_cpu(conexion_cpu, pcb_siguiente);;
				//send_CONTEXTO_EJECUCION(conexion_cpu, pcb_siguiente->contexto);// devolver contexto de ejecucion a cpu para que continue con la ejecucion del proceso
				log_info(log_kernel, "PID: <%d> - Eliminar Segmento - Id Segmento: <%d>",pcb_siguiente->contexto.PID, id_segmento);
			}
			else log_error(log_kernel, "Fallo recibiendo DELETE_SEGMENT");
		}


}
void manejar_otras_instrucciones(pcb_t* pcb_siguiente,uint32_t cop, float tiempoDeFin,uint32_t* seguir_ejecutando){
	uint32_t tiempo_bloqueo_kernel;
	 if (cop == IO)
			{

		 	 	 if (!recv_IO(conexion_cpu, &tiempo_bloqueo_kernel)) { //tiempo que pasar en block
		 				log_error(log_kernel, "Fallo recibiendo el tiempo bloqueante");
		 			}

		 	 	 else if(tiempo_bloqueo_kernel >= 0){// caso bloqueo, agrega a ready cuando se termina de bloquear
					pcb_siguiente->tiempo_bloqueo = tiempo_bloqueo_kernel;
					pthread_t hilo_Block;
					recalcular_rafagas_HRRN(pcb_siguiente, tiempoDeFin);
					log_info(log_kernel, "PID: <%d> - Ejecuta IO: <%d>", pcb_siguiente->contexto.PID, pcb_siguiente->tiempo_bloqueo);
					//hilo porque quiero I/O en paralelo

					pthread_create(&hilo_Block, NULL, (void*)bloquear_procesoPorIO,(void*)pcb_siguiente);
					pthread_detach(hilo_Block);
					noSalePorIO = false;
					send_seguir_ejecutando(conexion_cpu,1);
					}
			}
	 else if(cop == YIELD){
		recalcular_rafagas_HRRN(pcb_siguiente, tiempoDeFin);
		agregarAReady(pcb_siguiente);
		//send_seguir_ejecutando(conexion_cpu,1);

	 }
	 else if(cop == EXIT){// caso EXIT
		 	 	 	pcb_siguiente->motivo_exit = "SUCCESS";
					//send_seguir_ejecutando(conexion_cpu,1);
					terminarEjecucion(pcb_siguiente);

					//sem_post(&multiprogramacion); //le digo al new que ya puede mandar otro proceso mientras el grado de multiprog sea > 0
				}
	else if(cop==ERROR){
	pcb_siguiente->motivo_exit = "SEG_FAULT";
	terminarEjecucion(pcb_siguiente);
	}

}
void manejar_fileSystem(pcb_t* pcb_siguiente, uint32_t cop, float tiempoDeFin,uint32_t* seguir_ejecutando){
	if (cop == F_OPEN) // es un wait
				{
				    char* nombre_archivo;
				    if (recv_F_OPEN(conexion_cpu, &nombre_archivo))
				    {
				    	log_info(log_kernel,"PID: <%d> - Abrir Archivo: <%s>", pcb_siguiente->contexto.PID, nombre_archivo);
				    	fcb_por_proceso* archivo_proceso = malloc(sizeof(fcb_por_proceso));
				        fcb_kernel* archivo = encontrar_archivoTablaGlobal(tabla_ArchivosAbiertosGlobal, nombre_archivo);
				        if (archivo != NULL)
				        {// se agrega la entrada en la tabla de archivos abietos del proceso con el puntero en 0
				        	archivo_proceso->nombreArchivo = nombre_archivo;
				        	archivo_proceso->puntero = 0;
							list_add(pcb_siguiente->tabla_archivosAbiertos,archivo_proceso); // agrego a la tabla de archivos abiertos del proceso

							list_add(archivo->colaBloqueados, pcb_siguiente ); // cola de bloqueados del archivo, el pid del proceso que quiere usarlo
							pcb_siguiente->state_anterior = pcb_siguiente->state;
							pcb_siguiente->state = BLOCK;
							log_info(log_kernel, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <%s>",pcb_siguiente->contexto.PID,estado_pcb_a_string(pcb_siguiente->state_anterior),estado_pcb_a_string(pcb_siguiente->state));
							send_seguir_ejecutando(conexion_cpu,1);
							//pthread_mutex_unlock(&(archivo->mutexArchivo));
							recalcular_rafagas_HRRN(pcb_siguiente, tiempoDeFin);
				        }
				        else{
						log_info(log_kernel,"PID: <%d> - Abrir Archivo: <%s>", pcb_siguiente->contexto.PID, nombre_archivo);
							if(!send_EXISTE_ARCHIVO(conexion_fileSystem, nombre_archivo)){
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
									if(recv_OK_CODE(conexion_fileSystem,&estado))log_info(log_kernel, "FileSystem creo correctamente el archivo");
								}
								fcb_kernel* archivo_nuevo = malloc(sizeof(fcb_kernel));
								archivo_nuevo->nombreArchivo = nombre_archivo;
								archivo_nuevo->colaBloqueados = list_create();
								pthread_mutex_init(&archivo_nuevo->mutexArchivo, NULL);
								archivo_nuevo->tamanio = 0;

								archivo_proceso->puntero = 0;
								archivo_proceso->nombreArchivo = nombre_archivo;

								list_add(tabla_ArchivosAbiertosGlobal, archivo_nuevo);
								list_add(pcb_siguiente->tabla_archivosAbiertos,archivo_proceso); // agrego a la tabla de archivos abiertos del proceso
								send_seguir_ejecutando(conexion_cpu,0);
								//send_CONTEXTO_EJECUCION(conexion_cpu, pcb_siguiente->contexto);
							}

				    }
				    else log_error(log_kernel, "Fallo recibiendo FOPEN");
				}
	else if(cop == F_CLOSE)
	{
		char* nombre_archivo;

		if (recv_F_CLOSE(conexion_cpu, &nombre_archivo))
			{
			    fcb_kernel* archivo = encontrar_archivoTablaGlobal(tabla_ArchivosAbiertosGlobal,nombre_archivo);
			    if (archivo != NULL)
			    {
			    	uint32_t tamanioLista = list_size(archivo->colaBloqueados);
					if(tamanioLista > 0){ // algun proceso quiere ese archivo
					pcb_t* pcb_bloqueado= list_remove(archivo->colaBloqueados,0); //desbloqueo y despues lo mando a ready
					agregarAReady(pcb_bloqueado);
					}
					else{
						// ningun otro proceso quiere el archivo

					eliminarArchivoDeTablaGlobal(nombre_archivo, tabla_ArchivosAbiertosGlobal);// sacar el archivo de la tabla global tamanio
					}
					eliminarArchivoDeTablaProceso(nombre_archivo, pcb_siguiente->tabla_archivosAbiertos); //boorro el que lo tenia
					log_info(log_kernel,"PID: <%d> - Cerrar Archivo: <%s>", pcb_siguiente->contexto.PID, nombre_archivo);
			    }
			    send_seguir_ejecutando(conexion_cpu,0);
			}
		else log_error(log_kernel, "Fallo recibiendo FCLOSE");
	}

	else if (cop == F_SEEK){
		uint32_t posicion;
		char* nombre_archivo;
		if(recv_F_SEEK(conexion_cpu,&nombre_archivo,&posicion)){
			fcb_por_proceso* archivo = encontrar_archivoTablaProceso(pcb_siguiente->tabla_archivosAbiertos, nombre_archivo);
			archivo->puntero = posicion;
			send_seguir_ejecutando(conexion_cpu,0);
			//send_CONTEXTO_EJECUCION(conexion_cpu, pcb_siguiente->contexto);
			log_info(log_kernel,"PID: <%d> - Actualizar puntero Archivo: <%s> - Puntero <%d>", pcb_siguiente->contexto.PID, archivo->nombreArchivo, archivo->puntero);
		}

	}
	else if (cop == F_TRUNCATE){
		uint32_t tamanio;
		char* nombre_archivo;
		if(recv_F_TRUNCATE(conexion_cpu,&nombre_archivo,&tamanio)){
			send_F_TRUNCATE(conexion_fileSystem,nombre_archivo,tamanio);
			fcb_kernel* archivo = encontrar_archivoTablaGlobal(tabla_ArchivosAbiertosGlobal,nombre_archivo);
			archivo->tamanio = tamanio;
			 // cola de bloqueados del archivo, el pid del proceso que quiere usarlo
			list_add(archivo->colaBloqueados, pcb_siguiente );
		//	log_info(log_kernel, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <%s>",pcb_siguiente->contexto.PID,estado_pcb_a_string(pcb_siguiente->state_anterior),estado_pcb_a_string(pcb_siguiente->state));

			arg_archivo_bloqueado* args = malloc(sizeof(arg_archivo_bloqueado));
			pcb_siguiente->state_anterior = pcb_siguiente->state;
			pcb_siguiente->state = BLOCK;
			args->pcb = pcb_siguiente;
			args->archivo = archivo;
			args->nombre_instruccion = F_TRUNCATE;

			log_info(log_kernel,"PID: <%d> - Archivo: <%s> - Tamaño: <%d>", pcb_siguiente->contexto.PID, archivo->nombreArchivo,archivo->tamanio);
			pthread_t hilo_archivo;

			pthread_create(&hilo_archivo, NULL, (void*)bloquear_procesoPorArchivo,(void*)args);
			pthread_detach(hilo_archivo);
			send_seguir_ejecutando(conexion_cpu,1);
			recalcular_rafagas_HRRN(pcb_siguiente, tiempoDeFin);
		}
	}
	else if(cop ==F_READ){
		sem_wait(&semFRead);
		char* nombre_archivo;
		uint32_t direccion_fisica;
		uint32_t cantidad_bytes;

		if(!recv_F_READ(conexion_cpu, &nombre_archivo, &direccion_fisica, &cantidad_bytes)){
			log_error(log_kernel, "Fallo recibiendo F_READ");
		}
		else {
			fcb_por_proceso* archivo = encontrar_archivoTablaProceso(pcb_siguiente->tabla_archivosAbiertos, nombre_archivo);
			if(!send_F_READ(conexion_fileSystem, nombre_archivo, direccion_fisica, cantidad_bytes)){
				log_error(log_kernel, "Fallo enviando F_READ a FS");
			}
			if(!send_PUNTERO_FS(conexion_fileSystem, archivo->puntero)){
				log_error(log_kernel, "Fallo enviando Puntero a FS");
			}
			if(!send_PID(conexion_fileSystem, pcb_siguiente->contexto.PID)){
				log_error(log_kernel, "Fallo enviando PID a FS");
			}
			fcb_kernel* archivo_para_args = encontrar_archivoTablaGlobal(tabla_ArchivosAbiertosGlobal,nombre_archivo);
			list_add(archivo_para_args->colaBloqueados, pcb_siguiente );
			arg_archivo_bloqueado* args = malloc(sizeof(arg_archivo_bloqueado));
			pcb_siguiente->state_anterior = pcb_siguiente->state;
			pcb_siguiente->state = BLOCK;
			args->pcb = pcb_siguiente;
			args->archivo = archivo_para_args;
			args->nombre_instruccion = F_READ;

			log_info(log_kernel,"PID: <%d> - Leer Archivo: <%s> - Puntero <%d> - Dirección Memoria <%d> - Tamaño <%d>", pcb_siguiente->contexto.PID, archivo->nombreArchivo,archivo->puntero, direccion_fisica,archivo_para_args->tamanio);
			pthread_t hilo_archivo;
			pthread_create(&hilo_archivo, NULL, (void*)bloquear_procesoPorArchivo,(void*)args);
			pthread_detach(hilo_archivo);

			send_seguir_ejecutando(conexion_cpu,1);
			recalcular_rafagas_HRRN(pcb_siguiente, tiempoDeFin);
		}
	}


	else if(cop == F_WRITE){
		sem_wait(&semFWrite);
		char* nombre_archivo;
		uint32_t direccion_fisica;
		uint32_t cantidad_bytes;

		if(!recv_F_WRITE(conexion_cpu, &nombre_archivo, &direccion_fisica, &cantidad_bytes)){ //todo revisar que me llega la fisica
			log_error(log_kernel, "Fallo recibiendo F_WRITE");
		}
		else{
				if(!send_F_WRITE(conexion_fileSystem, nombre_archivo, direccion_fisica, cantidad_bytes)){
					log_error(log_kernel, "Fallo enviando F_WRITE a FS");
				}
				fcb_por_proceso* archivo = encontrar_archivoTablaProceso(pcb_siguiente->tabla_archivosAbiertos, nombre_archivo);
				if(!send_PUNTERO_FS(conexion_fileSystem, archivo->puntero)){
					log_error(log_kernel, "Fallo enviando Puntero a FS");
				}
				if(!send_PID(conexion_fileSystem, pcb_siguiente->contexto.PID)){
					log_error(log_kernel, "Fallo enviando PID a FS");
				}
				fcb_kernel* archivoTabla = encontrar_archivoTablaGlobal(tabla_ArchivosAbiertosGlobal,nombre_archivo);
				fcb_kernel* archivo_para_args = encontrar_archivoTablaGlobal(tabla_ArchivosAbiertosGlobal,nombre_archivo);
				list_add(archivo_para_args->colaBloqueados, pcb_siguiente );
				arg_archivo_bloqueado* args = malloc(sizeof(arg_archivo_bloqueado));
				pcb_siguiente->state_anterior = pcb_siguiente->state;
				pcb_siguiente->state = BLOCK;
				args->pcb = pcb_siguiente;
				args->archivo = archivoTabla;
				args->nombre_instruccion = F_WRITE;
				log_info(log_kernel,"PID: <%d> - Escribir Archivo: <%s> - Puntero <%d> - Dirección Memoria <%d> - Tamaño <%d>",pcb_siguiente->contexto.PID, archivo->nombreArchivo,archivo->puntero, direccion_fisica,archivoTabla->tamanio);
				pthread_t hilo_archivo;

				pthread_create(&hilo_archivo, NULL, (void*)bloquear_procesoPorArchivo,(void*)args);
				pthread_detach(hilo_archivo);

				send_seguir_ejecutando(conexion_cpu,1);
				recalcular_rafagas_HRRN(pcb_siguiente, tiempoDeFin);
		}
	}
}
void manejar_recursos(pcb_t* pcb_siguiente, uint32_t cop, float tiempoDeFin,uint32_t* seguir_ejecutando){
	if (cop == WAIT)
				{
				    char* nombre_recurso;
				    if (recv_WAIT(conexion_cpu, &nombre_recurso))
				    {
						recurso_sistema* recurso = encontrar_recurso(lista_recursos, nombre_recurso);
				        if (recurso != NULL)
				        {
				            if (recurso->instancia > 0){
				            	 log_info(log_kernel,"PID: <%d> - Wait: < %s > - Instancias: <%d>",pcb_siguiente->contexto.PID, recurso->nombre, recurso->instancia);
				                recurso->instancia--;
				                if(!encontrar_recurso(pcb_siguiente->recursos_asignados, nombre_recurso)) //si no encontro el recurso, agrega el recurso a la lista del proceso
				                asignar_recurso(nombre_recurso , pcb_siguiente->recursos_asignados);
				                else
				                aumentar_instancias_recurso(nombre_recurso , pcb_siguiente->recursos_asignados); // si ya tiene el recurso asignado, le sumo una instancia
				            send_seguir_ejecutando(conexion_cpu,0);
							}
				            else
				            {
				            	 log_info(log_kernel,"PID: <%d> - Wait: < %s > - Instancias: <%d>",pcb_siguiente->contexto.PID, recurso->nombre, recurso->instancia); //todo ver si va aca el log tmb
								recalcular_rafagas_HRRN(pcb_siguiente, tiempoDeFin);
				                // Bloquear el proceso actual en la cola de bloqueados del recurso
				                pthread_mutex_lock(&(recurso->mutexRecurso)); // creo que no es necesario el mutex, se comparte con otro hilo?
				                queue_push(recurso->colaBloqueados, pcb_siguiente);
				                pcb_siguiente->state_anterior = pcb_siguiente->state;
				                pcb_siguiente->state = BLOCK;
				                log_info(log_kernel,"PID: <%d> - Bloqueado por: <%s>",pcb_siguiente->contexto.PID,recurso->nombre);
				                log_info(log_kernel, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <%s>",pcb_siguiente->contexto.PID,estado_pcb_a_string(pcb_siguiente->state_anterior),estado_pcb_a_string(pcb_siguiente->state));
				                pthread_mutex_unlock(&(recurso->mutexRecurso)); // Desbloquear el acceso a la cola de bloqueados
				             send_seguir_ejecutando(conexion_cpu,1);
							}
				           // log_info(log_kernel,"PID: <%d> - Wait: < %s > - Instancias: <%d>",pcb_siguiente->contexto.PID, recurso->nombre, recurso->instancia);

						}
				        else { // si no existe el recurso
				        	pcb_siguiente->motivo_exit ="INVALID_RESOURCE";
							send_seguir_ejecutando(conexion_cpu,1);
				        	pcb_siguiente->finalizar_proceso = true;
				        	//terminarEjecucion(pcb_siguiente);
				        	//sem_post(&multiprogramacion);
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
				            log_info(log_kernel,"PID: <%d> - Signal: <%s> - Instancias: <%d>",pcb_siguiente->contexto.PID, recurso->nombre, recurso->instancia);
				            if (queue_size(recurso->colaBloqueados) > 0)
				            {
				                pthread_mutex_lock(&(recurso->mutexRecurso));
				                pcb_t* pcb_bloqueado = queue_pop(recurso->colaBloqueados);
				                recurso->instancia--;
				                pthread_mutex_unlock(&(recurso->mutexRecurso));
				                if(!encontrar_recurso(pcb_bloqueado->recursos_asignados, nombre_recurso)) //si no encontro el recurso, agrega el recurso a la lista del proceso
				                asignar_recurso(nombre_recurso , pcb_bloqueado->recursos_asignados);
				                 else
				                aumentar_instancias_recurso(nombre_recurso , pcb_bloqueado->recursos_asignados); // si ya tiene el recurso asignado, le sumo una instancia
				               	agregarAReady(pcb_bloqueado);
				            }
							//log_info(log_kernel,"PID: <%d> - Signal: <%s> - Instancias: <%d>",pcb_siguiente->contexto.PID, recurso->nombre, recurso->instancia);
						send_seguir_ejecutando(conexion_cpu,0);
						} else{
							pcb_siguiente->motivo_exit ="INVALID_RESOURCE";
						send_seguir_ejecutando(conexion_cpu,1);
							pcb_siguiente->finalizar_proceso = true;
							//terminarEjecucion(pcb_siguiente);
							//sem_post(&multiprogramacion);
						}
				    } else  log_error(log_kernel, "Fallo recibiendo SIGNAL");
				}

}
void manejar_contextosDeEjecucion(pcb_t* pcb_siguiente, contexto_ejecucion contexto, t_list* ts){ // maneja lo que  nos manda cpu
	uint32_t cop;

	if(recv_CONTEXTO_EJECUCION(conexion_cpu, &contexto)) // Las que recibimos que SI son instruccion
			{
				recv_TABLA_SEGMENTOS(conexion_cpu, &ts);
				if(recv(conexion_cpu, &cop, sizeof(op_code), 0) == sizeof(op_code)){
				pcb_siguiente->contexto= contexto;
				pcb_siguiente->contexto.TSegmento = ts;
				time_t fin_exe = time(NULL);
				float tiempoDeFin = ((float) fin_exe) * 1000;
				uint32_t seguir_ejecutando=1;
				if (pcb_siguiente->finalizar_proceso == false)
				{
				manejar_recursos(pcb_siguiente, cop, tiempoDeFin,&seguir_ejecutando);
				manejar_memoria(pcb_siguiente, cop,&seguir_ejecutando);
				manejar_fileSystem(pcb_siguiente, cop, tiempoDeFin,&seguir_ejecutando);
				manejar_otras_instrucciones(pcb_siguiente, cop, tiempoDeFin,&seguir_ejecutando);
				if(pcb_siguiente->finalizar_proceso == true){
				terminarEjecucion(pcb_siguiente);
				sem_post(&multiprogramacion);
					}
				}
			}

			}// del if del opcode
	else {
		log_error(log_kernel, "Fallo recibiendo el Contexto de Ejecucion");
	}
}// de la funcion
