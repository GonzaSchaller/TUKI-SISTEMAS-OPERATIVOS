#include "comunicacion.h"
extern t_log* logger;
extern t_config_fs *c;
extern int fd_fs;
//extern
extern t_bitarray* bitarray;

typedef struct{
	int fd;
	char*server_name;
} t_procesar_conexion_args;


static void procesar_conexionn(void* void_args){
	t_procesar_conexion_args*args = (t_procesar_conexion_args*) void_args;
	int cliente_socket = args->fd;
	char*server_name = args->server_name;
	free(args);


	op_code cop;

	while(cliente_socket!=-1) {
		if(recv(cliente_socket,&cop,sizeof(op_code),0) == 0){
			log_info(logger,"DISCONECT!");
			return;
		}


		switch(cop){
		// DEFINIR: ver como pasar los datos nombre archivo y tamanio porque despues lo usamos en config_set_value que sólo usa punteros

		//caso de F_OPEN (primero hay que ver si ya existe el archivo)
		case EXISTE_ARCHIVO:{
			extra_code estado;
			char*nombre_archivo = "hola.txt";
			uint32_t tamanio = 0; // DEFINIR ver si cambiarlo a char* o castear
			if(!recv_EXISTE_ARCHIVO(cliente_socket,&nombre_archivo))
				log_error(logger, "Fallo recibiendo f_open de kernel");

			log_info(logger,"abrir archivo %s",nombre_archivo);
			bool resultado = existe_y_abrir(nombre_archivo);

			if(!resultado){
				estado = INCORRECTO;
				send_OK_CODE(cliente_socket, estado);

				//crear el archivo.
				recv_CREAR_ARCHIVO(cliente_socket, &nombre_archivo, &tamanio); //para mi que no deberia recibir nada

				if(crear_archivo(nombre_archivo,tamanio)){
					estado = CORRECTO;
					log_info(logger,"cree el archivo");
					existe_y_abrir(nombre_archivo);
					send_OK_CODE(cliente_socket,estado);
				}

				else log_error(logger,"i cant del creararcguhjksd");
			}

			else {
				estado = CORRECTO;
				send_OK_CODE(cliente_socket, estado);
				log_info(logger,"el archivo existe");
			}
			break;
		}

		case F_WRITE:{
			char* nombre_archivo;
			uint32_t df; // la DF
			uint32_t cant_bytes; // cant bytes
			char* contenido;
			uint32_t puntero = 0;
			uint32_t estadok;
			uint32_t pid;

			recv_F_WRITE(cliente_socket,&nombre_archivo,&df,&cant_bytes);
			recv_PUNTERO_FS(cliente_socket,&puntero);
			recv_PID(cliente_socket,&pid);
			log_info(logger,"Escribir Archivo: <%s> - Puntero: <%d> - Memoria <%d> - Tamanio: <%d>",nombre_archivo,puntero,df,cant_bytes);

			//solicito a memoria lo que hay en la direccion logica mandada
			send_READ(cliente_socket,df,cant_bytes); // en caso de cpu seran tamanios de 4,8,16 bytes, en caso de filesystem no se sabe
			send_PID(cliente_socket,pid);

			//fcb_t* fcb = busacr_fcb(nombre_archivo);
			//aca recibo el contenido que le pediu a memoria
			recv_contenido_leido(fd_memoria,&contenido);

			if(escribir_contenido(nombre_archivo,contenido,puntero,cant_bytes))
				log_info(logger,"todo ok escribiendo el archivo");

			break;
		}

		case F_READ:{
			uint32_t estado_memoria;
			uint32_t estado_kernel;
			char* nombre_archivo;
			uint32_t df;
			uint32_t cant_bytes;
			uint32_t puntero = 0;

			recv_F_READ(cliente_socket,&nombre_archivo,&df,&cant_bytes);
			recv_PUNTERO_FS(cliente_socket,&puntero);
			log_info(logger,"Leer: Archivo: %s - Puntero: %d  - Memoria: <%d>  - Tamanio: <%d>",nombre_archivo,puntero,df,cant_bytes);
			char*contenidor = buscar_contenido(nombre_archivo,puntero,cant_bytes);

			//le mando a memoria lo que tiene que escribir
			if(contenidor != NULL){
				send_WRITE(fd_memoria,df,contenidor);
				recv_OK_CODE(fd_memoria,&estado_memoria);
				if(estado_memoria == EXITOSO){
					estado_kernel = EXITOSO;
					send_OK_CODE(cliente_socket,estado_kernel);
				}
			}
			log_error(logger, "Error al abrir el archivo por que no se pudo obtener el contenido del mismo");

			break;
		}

		case F_TRUNCATE:{
			char* nombre_archivo;
			uint32_t tamanio_a_truncar;

			if(!recv_F_TRUNCATE(cliente_socket, &nombre_archivo, &tamanio_a_truncar)) log_error(logger,"error al recibir F_TRUNCATE de kernel");

			char* path = concat(nombre_archivo);
			fcb_t* fcb = malloc(sizeof(fcb));
			t_config* archivo = config_create(path);

			fcb->puntero_directo = config_get_int_value(archivo, "PUNTERO_DIRECTO");
			fcb->puntero_indirecto = config_get_int_value(archivo, "PUNTERO_INDIRECTO");
			fcb->tamanio_archivo = config_get_int_value(archivo, "TAMANIO_ARCHIVO");
			fcb->nombreArchivo = config_get_string_value(archivo, "TAMANIO_ARCHIVO");

			uint32_t cuantos_bloques_venia_usando = ceil_casero(fcb->tamanio_archivo,superbloque->block_size);

			//tengo que agregar una cantidad mas de bloques.
			if(tamanio_a_truncar > fcb->tamanio_archivo){
				aumentar_tamanio_archivo(tamanio_a_truncar, cuantos_bloques_venia_usando, fcb, archivo);
				/*
				uint32_t cantidad_bloques_con_nuevo_tamanio = ceil_casero(tamanio_a_truncar,superbloque->block_size); //cantidad de bloques necesarios para direccionar el nuevo tamanio del archivo.
				uint32_t cantidad_bloques_a_agregar = cantidad_bloques_con_nuevo_tamanio - cuantos_bloques_venia_usando;

				if(fcb->puntero_directo != -1){ //necesito un bloque mas que sera el indirecto
					cantidad_bloques_a_agregar ++;
				}

				t_list*list_nro_de_bloques = list_create();
				uint32_t bloques_totales_fs = bitarray_get_max_bit(bitarray);

				for(int i = 0; i < bloques_totales_fs; i++){ //recorro el bitarray en busca de bloques libres, necesito solo cant_bloques_Agregar
					uint32_t tamanio= list_size(list_nro_de_bloques);

					if(tamanio == cantidad_bloques_a_agregar)
						return;

					else {
						bool valor = bitarray_test_bit(bitarray,i);
						if(valor)
							list_add(list_nro_de_bloques,(void*)i);
					}

					if((i == (bloques_totales_fs - 1)) && tamanio!= cantidad_bloques_a_agregar)
						log_error(logger,"NO HAY MAS BLOQUES DISPONIBLES PARA AGRANDAR EL TAMANIO DEL ARCHIVO");
				}

				//SALGO DEL FOR CON LA LISTA DE LOS BLOQUES A AGREGAR, si termine antes es que no habia bloques :(

				//seteo el tamanio
				char tamanio_str[20];
				sprintf(tamanio_str, "%d", tamanio_a_truncar);

				config_set_value(archivo,"TAMANIO_ARCHIVO",tamanio_str);

				for(int i=0;i < cantidad_bloques_a_agregar;i++){
					if(fcb->puntero_directo == -1){ //ese archivo era nuevo y no tenia bloques asignados
						fcb->puntero_directo = (uint32_t)list_get(list_nro_de_bloques,i);
						char puntero_str[20];
						sprintf(puntero_str, "%d", fcb->puntero_directo);
						config_set_value(archivo,"PUNTERO_DIRECTO",puntero_str);
						cantidad_bloques_a_agregar--;
					}

					else if (fcb->puntero_indirecto == -1) {
						// antes no tenia punteros indirectos.

						uint32_t bloque = (uint32_t)list_get(list_nro_de_bloques,i);
						fcb->puntero_indirecto = bloque;
						char puntero_str[20];
						sprintf(puntero_str, "%d",fcb->puntero_indirecto);
						config_set_value(archivo,"PUNTERO_INDIRECTO",puntero_str); //seteo el puntero indirecto
						cantidad_bloques_a_agregar--;
						fseek(f_bloques, fcb->puntero_indirecto*superbloque->block_size, SEEK_SET); // me muevo al principio del puntero indirecto
					}

					// si entro aca es que ya tiene asignado el bloque directo y el indirecto,
				 	// entonces solo queda escribir en el archivo de bloques los bloques que quedan
					else {
						uint32_t bloque = (uint32_t)list_get(list_nro_de_bloques,i);
						fseek(f_bloques, fcb->puntero_indirecto*superbloque->block_size + (i*sizeof(uint32_t)), SEEK_SET); // la cuenta es asi porque cada vez que se mueve lo hace con un offset
						fwrite(bloque, 1,sizeof(uint32_t), f_bloques);

						//logs: // AGREGAR LOGS

					}
				} //llave del for, linea 177
			} //llave del if si hay que agregar bloques, linea 142
*/
			}

			else{ //es mas chico
				reducir_tamanio_archivo(tamanio_a_truncar, cuantos_bloques_venia_usando, fcb, archivo);
				/*
				uint32_t cantidad_bloques_con_nuevo_tamanio = ceil_casero(tamanio_a_truncar,superbloque->block_size); //cantidad de bloques necesarios para direccionar el nuevo tamanio del archivo.
				uint32_t cantidad_bloques_a_deletear_neto =  cuantos_bloques_venia_usando - cantidad_bloques_con_nuevo_tamanio ; //ESTOS SON LOS NETOS SIN PUNTERO INDIRECTOS
				uint32_t cantidad_bloques_a_deletear_mas_pi =  cuantos_bloques_venia_usando - cantidad_bloques_con_nuevo_tamanio ;

				uint32_t cuantos_bloques_venia_usando_sin_pi;

				// tengo que recorrer mis punteros y agregar mis bloques en una listaosea como la operacion al reves de lo que hice arriba.

				uint32_t puntero_indirecto = fcb->puntero_indirecto;
				if(puntero_indirecto != - 1){
					cuantos_bloques_venia_usando++; //contiene al indirecto
				}

				//tengo que parsear el archivo de bloques para agarrar los bloques.
				fseek(f_bloques, sizeof(uint32_t)*superbloque->block_size, SEEK_SET); // me paro en el bloquie indirecto para empezar a parsear

				t_list*list_nro_de_bloques = list_create();

				uint32_t cant_bloques_en_bloque_indirecto = 0;
				list_add(list_nro_de_bloques,fcb->puntero_directo); //aniadi el punero directo a la lista, es el primer bloque
				if(puntero_indirecto !=-1){
					list_add(list_nro_de_bloques,puntero_indirecto); // aniado al punero indirecto a la lista

					for(int i=0;i<cuantos_bloques_venia_usando-2;i++){ // le resto el directo y el indirecto y agrego los bloques que apunta el puntero indirecto osea la lista quedaria BLOQUE DIRECTO (puntero directo) + BLOQUE INDIRECTO (puntero al bloque indirecto) + BLOQUES DIRECTOS(punteros a los bloques directos que apunta el puntero directo)
						uint32_t contenido_leido = malloc(sizeof(uint32_t));
						fread(contenido_leido, sizeof(uint32_t),1, f_bloques);
						list_add(list_nro_de_bloques,contenido_leido);
						cant_bloques_en_bloque_indirecto++;
					}
				} //SALE DE ACA CON UNA LISTA DE TODOS LOS BLOQUES QUE TIENE

				//AHORA TENGO QUE FIJARME EL BITARRAY
				list_sort(list_nro_de_bloques,reverse_compare); //doy vuelta la lista

				for(int i=0;i<cant_bloques_en_bloque_indirecto;i++){
					if(fcb->puntero_indirecto!=-1){

						// si la cantidad de bloques a eliminar es mayor a la cantidad de bloques que tengo
						// en mis bloques indirectos, significa que tengo que eliminar entero el bloque indirecto + la
						// cantidad de bloques indirectos usados
						if(cantidad_bloques_a_deletear_neto >= cant_bloques_en_bloque_indirecto){

							//borro la canridad de bloques del indirecto+1 que es el bloque indirecto y seteo al bloque indirecto con -1
							for(int i=0;i<cantidad_bloques_a_deletear_neto+1;i++){
								// recorro el bitarray desde la posicion que hay en la lista
								uint32_t bloque = list_get(list_nro_de_bloques,i);
								//bit_clean(bitarray,bloque);
								bitarray_clean_bit(bitarray,bloque); //puede ser? cambie al de arriba por este

								cantidad_bloques_a_deletear_neto -= cant_bloques_en_bloque_indirecto;
								cant_bloques_en_bloque_indirecto = 0;
								//	list_remove()

								int defecto = -1;
								char puntero_str[20];
								sprintf(puntero_str, "%d",defecto);
								config_set_value(archivo,"PUNTERO_INDIRECTO",puntero_str); //seteo el puntero indirecto
							}
						}

						// si lo que tengo que borrar esta en el bloque indirecto y es menor a los que hay, entonces
						// borro los que hay, no hay que borrar el puntero indirecto
						else if(cantidad_bloques_a_deletear_neto<cant_bloques_en_bloque_indirecto){
							for(int i=0;i<cantidad_bloques_a_deletear_neto;i++){
								// recorro el bitarray desde la posicion que hay en la lista
								uint32_t bloque = list_get(list_nro_de_bloques,i);
								//bit_clean(bitarray,bloque);
								bitarray_clean_bit(bitarray, bloque); //CONFIRMAR QUE ES ESTA FUNCION Y NO LA DE ARRIBA
								cantidad_bloques_a_deletear_neto-=cant_bloques_en_bloque_indirecto;
							}

						}

						else {// solo tengo un puntero directo, entonces lo que va a borrar es el puntero directo.
							uint32_t bloque = list_get(list_nro_de_bloques,0); // si se borro todo bien solo quedaria en la posicion 0 el punetero directo
							//bit_clean(bitarray,bloque);
							bitarray_clean_bit(bitarray, bloque); //CONFIRMAR QUE ES LA FUNCION CORRECTA

							//setear en -1
							int defecto = -1;
							char puntero_str[20];
							sprintf(puntero_str, "%d",defecto);
							config_set_value(archivo,"PUNTERO_DIRECTO",puntero_str); //seteo el puntero indirecto
						}
					} //llave del if, linea 247
				} //llave del for de borrar, linea 246
*/
			}//else de mas chico, linea 211
			break;
		} //ftruncate, linea 124

		log_warning(logger,"cliente %s desconectado ",server_name);
		} //llave del switch de arriba, linea 30

	} //cierra el primer while, linea 23
}

void aumentar_tamanio_archivo(uint32_t tamanio_a_truncar, uint32_t cuantos_bloques_venia_usando, fcb_t* fcb, t_config* archivo){
	uint32_t cantidad_bloques_con_nuevo_tamanio = ceil_casero(tamanio_a_truncar,superbloque->block_size); //cantidad de bloques necesarios para direccionar el nuevo tamanio del archivo.
					uint32_t cantidad_bloques_a_agregar = cantidad_bloques_con_nuevo_tamanio - cuantos_bloques_venia_usando;

					if(fcb->puntero_directo != -1){ //necesito un bloque mas que sera el indirecto
						cantidad_bloques_a_agregar ++;
					}

					t_list*list_nro_de_bloques = list_create();
					uint32_t bloques_totales_fs = bitarray_get_max_bit(bitarray);

					for(int i = 0; i < bloques_totales_fs; i++){ //recorro el bitarray en busca de bloques libres, necesito solo cant_bloques_Agregar
						uint32_t tamanio= list_size(list_nro_de_bloques);

						if(tamanio == cantidad_bloques_a_agregar)
							return;

						else {
							bool valor = bitarray_test_bit(bitarray,i);
							if(valor)
								list_add(list_nro_de_bloques,(void*)i);
						}

						if((i == (bloques_totales_fs - 1)) && tamanio!= cantidad_bloques_a_agregar)
							log_error(logger,"NO HAY MAS BLOQUES DISPONIBLES PARA AGRANDAR EL TAMANIO DEL ARCHIVO");
					}

					//SALGO DEL FOR CON LA LISTA DE LOS BLOQUES A AGREGAR, si termine antes es que no habia bloques :(

					//seteo el tamanio
					char tamanio_str[20];
					sprintf(tamanio_str, "%d", tamanio_a_truncar);

					config_set_value(archivo,"TAMANIO_ARCHIVO",tamanio_str);

					for(int i=0;i < cantidad_bloques_a_agregar;i++){
						if(fcb->puntero_directo == -1){ //ese archivo era nuevo y no tenia bloques asignados
							fcb->puntero_directo = (uint32_t)list_get(list_nro_de_bloques,i);
							char puntero_str[20];
							sprintf(puntero_str, "%d", fcb->puntero_directo);
							config_set_value(archivo,"PUNTERO_DIRECTO",puntero_str);
							cantidad_bloques_a_agregar--;
						}

						else if (fcb->puntero_indirecto == -1) {
							// antes no tenia punteros indirectos.

							uint32_t bloque = (uint32_t)list_get(list_nro_de_bloques,i);
							fcb->puntero_indirecto = bloque;
							char puntero_str[20];
							sprintf(puntero_str, "%d",fcb->puntero_indirecto);
							config_set_value(archivo,"PUNTERO_INDIRECTO",puntero_str); //seteo el puntero indirecto
							cantidad_bloques_a_agregar--;
							fseek(f_bloques, fcb->puntero_indirecto*superbloque->block_size, SEEK_SET); // me muevo al principio del puntero indirecto
						}

						// si entro aca es que ya tiene asignado el bloque directo y el indirecto,
					 	// entonces solo queda escribir en el archivo de bloques los bloques que quedan
						else {
							uint32_t bloque = (uint32_t)list_get(list_nro_de_bloques,i);
							fseek(f_bloques, fcb->puntero_indirecto*superbloque->block_size + (i*sizeof(uint32_t)), SEEK_SET); // la cuenta es asi porque cada vez que se mueve lo hace con un offset
							fwrite(bloque, 1,sizeof(uint32_t), f_bloques);

							//logs: // AGREGAR LOGS

						}
					} //llave del for
}


void reducir_tamanio_archivo(uint32_t tamanio_a_truncar, uint32_t cuantos_bloques_venia_usando, fcb_t* fcb, t_config* archivo){
	uint32_t cantidad_bloques_con_nuevo_tamanio = ceil_casero(tamanio_a_truncar,superbloque->block_size); //cantidad de bloques necesarios para direccionar el nuevo tamanio del archivo.
					uint32_t cantidad_bloques_a_deletear_neto =  cuantos_bloques_venia_usando - cantidad_bloques_con_nuevo_tamanio ; //ESTOS SON LOS NETOS SIN PUNTERO INDIRECTOS
					uint32_t cantidad_bloques_a_deletear_mas_pi =  cuantos_bloques_venia_usando - cantidad_bloques_con_nuevo_tamanio ;

					uint32_t cuantos_bloques_venia_usando_sin_pi;

					// tengo que recorrer mis punteros y agregar mis bloques en una listaosea como la operacion al reves de lo que hice arriba.

					uint32_t puntero_indirecto = fcb->puntero_indirecto;
					if(puntero_indirecto != - 1){
						cuantos_bloques_venia_usando++; //contiene al indirecto
					}

					//tengo que parsear el archivo de bloques para agarrar los bloques.
					fseek(f_bloques, sizeof(uint32_t)*superbloque->block_size, SEEK_SET); // me paro en el bloquie indirecto para empezar a parsear

					t_list*list_nro_de_bloques = list_create();

					uint32_t cant_bloques_en_bloque_indirecto = 0;
					list_add(list_nro_de_bloques,fcb->puntero_directo); //aniadi el punero directo a la lista, es el primer bloque
					if(puntero_indirecto !=-1){
						list_add(list_nro_de_bloques,puntero_indirecto); // aniado al punero indirecto a la lista

						for(int i=0;i<cuantos_bloques_venia_usando-2;i++){ // le resto el directo y el indirecto y agrego los bloques que apunta el puntero indirecto osea la lista quedaria BLOQUE DIRECTO (puntero directo) + BLOQUE INDIRECTO (puntero al bloque indirecto) + BLOQUES DIRECTOS(punteros a los bloques directos que apunta el puntero directo)
							uint32_t contenido_leido = malloc(sizeof(uint32_t));
							fread(contenido_leido, sizeof(uint32_t),1, f_bloques);
							list_add(list_nro_de_bloques,contenido_leido);
							cant_bloques_en_bloque_indirecto++;
						}
					} //SALE DE ACA CON UNA LISTA DE TODOS LOS BLOQUES QUE TIENE

					//AHORA TENGO QUE FIJARME EL BITARRAY
					list_sort(list_nro_de_bloques,reverse_compare); //doy vuelta la lista

					for(int i=0;i < cant_bloques_en_bloque_indirecto;i++){
						if(fcb->puntero_indirecto!=-1){

							/* si la cantidad de bloques a eliminar es mayor a la cantidad de bloques que tengo
							en mis bloques indirectos, significa que tengo que eliminar entero el bloque indirecto + la
							cantidad de bloques indirectos usados  */
							if(cantidad_bloques_a_deletear_neto >= cant_bloques_en_bloque_indirecto){

								//borro la canridad de bloques del indirecto+1 que es el bloque indirecto y seteo al bloque indirecto con -1
								for(int i=0;i<cantidad_bloques_a_deletear_neto+1;i++){
									// recorro el bitarray desde la posicion que hay en la lista
									uint32_t bloque = list_get(list_nro_de_bloques,i);
									//bit_clean(bitarray,bloque);
									bitarray_clean_bit(bitarray,bloque); //puede ser? cambie al de arriba por este

									cantidad_bloques_a_deletear_neto -= cant_bloques_en_bloque_indirecto;
									cant_bloques_en_bloque_indirecto = 0;
									//	list_remove()

									int defecto = -1;
									char puntero_str[20];
									sprintf(puntero_str, "%d",defecto);
									config_set_value(archivo,"PUNTERO_INDIRECTO",puntero_str); //seteo el puntero indirecto
								}
							}

							/* si lo que tengo que borrar esta en el bloque indirecto y es menor a los que hay, entonces
							borro los que hay, no hay que borrar el puntero indirecto*/
							else if(cantidad_bloques_a_deletear_neto < cant_bloques_en_bloque_indirecto){
								for(int i=0;i<cantidad_bloques_a_deletear_neto;i++){
									// recorro el bitarray desde la posicion que hay en la lista
									uint32_t bloque = list_get(list_nro_de_bloques,i);
									//bit_clean(bitarray,bloque);
									bitarray_clean_bit(bitarray, bloque); //CONFIRMAR QUE ES ESTA FUNCION Y NO LA DE ARRIBA
									cantidad_bloques_a_deletear_neto -= cant_bloques_en_bloque_indirecto;
								}

							}

							else {// solo tengo un puntero directo, entonces lo que va a borrar es el puntero directo.
								uint32_t bloque = list_get(list_nro_de_bloques,0); // si se borro bien solo quedaria en la posicion 0 el puntero directo
								//bit_clean(bitarray,bloque);
								bitarray_clean_bit(bitarray, bloque); //CONFIRMAR QUE ES LA FUNCION CORRECTA

								//setear en -1
								int defecto = -1;
								char puntero_str[20];
								sprintf(puntero_str, "%d",defecto);
								config_set_value(archivo,"PUNTERO_DIRECTO",puntero_str); //seteo el puntero indirecto
							}
						} //llave del if, linea 247
					}
}


int generar_conexion_con_memoria(){
	int conexion = crear_conexion(logger,"File system",c->ip_memoria, c->puerto_memoria);
	uint8_t handshake =1;
	uint8_t result;
	send_handshake(conexion,handshake);
	recv_handshake(conexion,&result);
	if(result == 1) log_info(logger,"todo ok capo");
	return conexion;
}

void conexion_kernel(t_log* log_memoria,char* server_name, int server_socket){
	int fd_kernel = esperar_cliente(logger, fd_fs);
		if(fd_kernel!=-1){
			if (fd_kernel != -1) {
			       pthread_t hilo;
			       t_procesar_conexion_args* args = malloc(sizeof(t_procesar_conexion_args));
			       args->fd = fd_kernel;
			       args->server_name = server_name;
			       pthread_create(&hilo, NULL, (void*) procesar_conexionn, (void*) args);
			       pthread_detach(hilo);
			   }
		}

}












