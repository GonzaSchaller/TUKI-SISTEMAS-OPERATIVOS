#include "comunicacion.h"
extern t_log* logger;
extern t_config_fs *c;
extern int fd_fs;
//extern


void procesar_peticiones(int cliente_socket){
	op_code cop;
	while(cliente_socket!=-1){
		if(recv(cliente_socket,&cop,sizeof(op_code),0) == 0){
			log_info(logger,"DISCONECT!");
				return;  }


				switch(cop){
				// DEFINIR: ver como pasar los datos nombre archivo y tamanio porque despues lo usamos en config_set_value que sólo usa punteros

				//caso de F_OPEN primero hay que ver si ya existe el archivo
				case EXISTE_ARCHIVO:{
					extra_code estado;
					char*nombre_archivo;
					uint32_t tamanio; // DEFINIR ver si cambiarlo a char* o castear
					if(!recv_EXISTE_ARCHIVO(cliente_socket,&nombre_archivo)) log_error(logger, "Fallo recibiendo f_open de kernel");

					log_info(logger,"abrir archivo %s",nombre_archivo);
					bool resultado = existe_y_abrir(nombre_archivo);
					if(!resultado){
						estado = INCORRECTO;
						send_OK_CODE(cliente_socket, estado);
						//crear el arcchivo.
						recv_CREAR_ARCHIVO(cliente_socket, &nombre_archivo, &tamanio); //para mi que no deberia recibir nada

						if(crear_archivo(nombre_archivo,tamanio)){
							estado = CORRECTO;
							send_OK_CODE(cliente_socket,estado);
						}
					}
					else{
						estado = CORRECTO;
						send_OK_CODE(cliente_socket, estado);
					}
				}
				break;


				case F_WRITE:{
					char* nombre_archivo;
				    uint32_t df; // la DF
					uint32_t cant_bytes; // cant bytes
					char*contenido;
					uint32_t puntero = 0;
					uint32_t estadok;
					uint32_t pid;

					recv_F_WRITE(cliente_socket,&nombre_archivo,&df,&cant_bytes);
					recv_PUNTERO_FS(cliente_socket,puntero);

					log_info(logger,"Escribir Archivo: <%s> - Puntero: <%d> - Memoria <%d> - Tamanio: <%d>",nombre_archivo,puntero,df,cant_bytes);

					//solicito a memoria lo que hay en la direccion logica mandada
					send_READ(cliente_socket,df,cant_bytes); // en caso de cpu seran tamanios de 4,8,16 bytes, en caso de filesystem no se sabe
					send_PID(cliente_socket,pid);

					//aca recibo el contenido que le pediu a memoria
					recv_contenido_leido(fd_memoria,&contenido);


					if(escribir_contenido(contenido,puntero)){
						log_info(logger,"todo ok escribiendo el archivo");
					}
				   }
					   break;
				case F_READ:{
					uint32_t estado_memoria;
					uint32_t estado_kernel;
					char* nombre_archivo;
					uint32_t df;
					uint32_t cant_bytes;
					uint32_t puntero=0;


					recv_F_READ(cliente_socket,&nombre_archivo,&df,&cant_bytes);
					recv_PUNTERO_FS(cliente_socket,&puntero);
					log_info(logger,"Leer: Archivo: %s - Puntero: %d  - Memoria: <%d>  - Tamanio: <%d>",nombre_archivo,puntero,df,cant_bytes);
					char*contenidor = buscar_contenido(puntero,cant_bytes);
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
					char* nombre_archivo; //TODO DUDAS como es una variable deberia ser una general en vez de hacer varias para cada case
					uint32_t tamanio;
					recv_F_TRUNCATE(cliente_socket, &nombre_archivo, &tamanio);

					char* path = concat(nombre_archivo); //TODO DUDAS (CONCAT): ver si nombre_archivo va asÃ­ o con el * o &
					t_config* archivo = config_create(path);
					int pd = config_get_int_value(archivo, "PUNTERO_DIRECTO");
					int pi = config_get_int_value(archivo, "PUNTERO_INDIRECTO");

					uint32_t tamanio_archivo = config_get_int_value(archivo, "TAMANIO_ARCHIVO");
					if(tamanio > tamanio_archivo){

					uint32_t cantidadBloques = (uint32_t)ceil((double)tamanio / superbloque->block_size); //bloques a agregar
					for(int i = 0; i<bitarray_get_max_bit(bitarray); i++){
						uint32_t bloques_asignados = 0;
						if(!bitarray_test_bit(bitarray, i) && bloques_asignados != cantidadBloques){ // si el bit es 0 y todavia me faltan bloques por asignar
							bitarray_set_bit(bitarray, i);
							char buffer[5];
							sprintf(buffer, "%d", i); //chequear

						if(pd == -1){ // no tiene puntero directo asginado
						config_set_value(archivo,"PUNTERO_DIRECTO",buffer);
						}
						else if(pi == -1){ //no tiene puntero indirecto
							config_set_value(archivo,"PUNTERO_INDIRECTO",buffer);
						}
						else if(pi != -1){ //ya tiene un puntero indirecto
						//hay que ver que puntero es y agregarle un puntero a un bloque (Ni idea como hacerlo)
												}
							bloques_asignados++;
						}
						}
						}
							else{ //es mas chico
								int tamanioASacar = tamanio_archivo - tamanio;
								int bloquesASacar = (uint32_t)ceil((double)tamanioASacar / superbloque->block_size); //bloques a agregar

								for(int i = 0; i<bitarray_get_max_bit(bitarray); i++){
											uint32_t bloques_sacados = 0;
											if(bloques_sacados != bloquesASacar){
												//me fijo el puntero indirecto, entro al bloque y me fijo la cantidad de punteros a bloques y cuales son
												// voy sacando uno por uno del bitmap -> bitarray_clean_bit
												bloques_sacados++;
											}
										}

									}
									config_set_value(archivo,"TAMANIO_ARCHIVO",tamanio);

										break;

								}

					//uso funcion concat para obtener el path y asi usar c
					//TODO DUDAS no se como llegar al FCB

					// var pasada por parametro: el nuevo tamanio
					// a partir de ahí ver si es más chico o más grande y hacer lo necesario:
						//TODO TRUNCATE: Actualizar el tamanio del archivo en FCB
						// Si es mas chico que el de antes:
							//Marcar libres los bloques (no hace falta borrar el contenido)
						// Si es más grande que el de antes:
							//TODO Asignar nuevos bloques (ver que significa)
						break;
				}

				}

}
}


int generar_conexion_con_memoria(){
	int conexion = crear_conexion(logger,"Memoria",c->ip_memoria, c->puerto_memoria);
	uint8_t handshake =1;
	uint8_t result;
	send_handshake(conexion,handshake);
	recv_handshake(conexion,&result);
	if(result == 1) log_info(logger,"todo ok capo");
	return conexion;
}


void conexion_kernel(){
	int fd_kernel = esperar_cliente(logger, fd_fs);
		if(fd_kernel!=-1){
			procesar_peticiones(fd_kernel);
		}
}












