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

			log_info(logger,"Abrir archivo: <%s>",nombre_archivo);
			bool resultado = existe_y_abrir(nombre_archivo);

			if(!resultado){
				estado = INCORRECTO;
				send_OK_CODE(cliente_socket, estado);

				//crear el archivo.
				recv_CREAR_ARCHIVO(cliente_socket, &nombre_archivo, &tamanio); //para mi que no deberia recibir nada

				log_info(logger,"Crear Archivo: <%s>", nombre_archivo);
				if(crear_archivo(nombre_archivo,tamanio)){
					estado = CORRECTO;
					existe_y_abrir(nombre_archivo);
					send_OK_CODE(cliente_socket,estado);
				}

				else log_error(logger,"Error al crear el Archivo <%s>", nombre_archivo);
			}

			else {
				estado = CORRECTO;
				send_OK_CODE(cliente_socket, estado);
				//log_info(logger,"El archivo <%s> existe", nombre_archivo);
			}
			break;
		}

		case F_WRITE:{
			char* nombre_archivo;
			uint32_t df; // la DF
			uint32_t cant_bytes; // cant bytes
			char* contenido = "";
			uint32_t puntero = 0;
			uint32_t estadok;
			uint32_t pid;

			recv_F_WRITE(cliente_socket,&nombre_archivo,&df,&cant_bytes);
			recv_PUNTERO_FS(cliente_socket,&puntero);
			recv_PID(cliente_socket,&pid);
			log_info(logger,"Escribir Archivo: <%s> - Puntero: <%d> - Memoria <%d> - Tamanio: <%d>",nombre_archivo,puntero,df,cant_bytes);

			//solicito a memoria lo que hay en la direccion logica mandada
			send_READ_FS(fd_memoria,df,cant_bytes); // en caso de cpu seran tamanios de 4,8,16 bytes, en caso de filesystem no se sabe
			send_PID(fd_memoria,pid);
			//fcb_t* fcb = busacr_fcb(nombre_archivo);
			//aca recibo el contenido que le pediu a memoria
			if(!recv_contenido_leido(fd_memoria,&contenido)){
				log_info(logger, "Fallo recibiendo contenido de memoria");
			}

			if(escribir_contenido(nombre_archivo,contenido,puntero,cant_bytes)){
				//log_info(logger,"Todo ok escribiendo el archivo");
				extra_code ok = FINALIZAR;
				send_OK_CODE(cliente_socket, ok);
			}
			else
				log_error(logger, "No se pudo escribir en el archivo");
			break;
		}

		case F_READ:{
			uint32_t estado_memoria;
			uint32_t estado_kernel;
			char* nombre_archivo;
			uint32_t df;
			uint32_t cant_bytes;
			uint32_t puntero = 0;
			uint32_t pid;
			recv_F_READ(cliente_socket,&nombre_archivo,&df,&cant_bytes);
			recv_PUNTERO_FS(cliente_socket,&puntero);
			recv_PID(cliente_socket, &pid);

			log_info(logger,"Leer Archivo: <%s> - Puntero: <%d>  - Memoria: <%d>  - Tamanio: <%d>",nombre_archivo,puntero,df,cant_bytes);
			char*contenidor = buscar_contenido(nombre_archivo,puntero,cant_bytes);

			//le mando a memoria lo que tiene que escribir
			if(contenidor != NULL){
				send_WRITE_FS(fd_memoria,df,contenidor);
				send_cant_bytes(fd_memoria, cant_bytes);
				send_PID(fd_memoria, pid);
 				recv_OK_CODE(fd_memoria,&estado_memoria);
				if(estado_memoria == EXITOSO){
					extra_code ok = FINALIZAR;
					send_OK_CODE(cliente_socket, ok);
				}
			}

			break;
		}

		case F_TRUNCATE:{
			char* nombre_archivo;
			uint32_t tamanio_a_truncar;

			if(!recv_F_TRUNCATE(cliente_socket, &nombre_archivo, &tamanio_a_truncar)) log_error(logger,"error al recibir F_TRUNCATE de kernel");

			log_info(logger,"Truncar Archivo: <%s> - Tamanio: <%d>", nombre_archivo, tamanio_a_truncar);

			char* path = concat(nombre_archivo);
			fcb_t* fcb = malloc(sizeof(fcb_t));
			t_config* archivo = config_create(path);

			fcb->puntero_directo = config_get_int_value(archivo, "PUNTERO_DIRECTO");
			fcb->puntero_indirecto = config_get_int_value(archivo, "PUNTERO_INDIRECTO");
			fcb->tamanio_archivo = config_get_int_value(archivo, "TAMANIO_ARCHIVO");
			fcb->nombreArchivo =  strdup(config_get_string_value(archivo, "NOMBRE_ARCHIVO"));


			uint32_t cuantos_bloques_venia_usando = ceil_casero(fcb->tamanio_archivo,superbloque->block_size);

			//tengo que agregar una cantidad mas de bloques.
			if(tamanio_a_truncar > fcb->tamanio_archivo){
				aumentar_tamanio_archivo(tamanio_a_truncar, cuantos_bloques_venia_usando , fcb, archivo);
			}
			else if(tamanio_a_truncar < fcb->tamanio_archivo){ //tengo que quitar bloques al archivo
				reducir_tamanio_archivo(tamanio_a_truncar, cuantos_bloques_venia_usando, fcb, archivo);
			} //ftruncate, linea 124

			extra_code ok = FINALIZAR;
			send_OK_CODE(cliente_socket, ok);
			break;
		}
		log_warning(logger,"cliente %s desconectado ",server_name);
		} //llave del switch de arriba, linea 30

	} //cierra el primer while, linea 23
}

void aumentar_tamanio_archivo(uint32_t tamanio_a_truncar, uint32_t cuantos_bloques_venia_usando, fcb_t* fcb, t_config* archivo){

	//cantidad de bloques necesarios para direccionar el nuevo tamanio del archivo.
	uint32_t cantidad_bloques_con_nuevo_tamanio = ceil_casero(tamanio_a_truncar,superbloque->block_size);
	uint32_t cantidad_bloques_a_agregar = cantidad_bloques_con_nuevo_tamanio - cuantos_bloques_venia_usando;

	uint32_t bloques_totales_fs = bitarray_get_max_bit(bitarray);

	if(fcb->tamanio_archivo == 0){
		llenar_archivo_nuevo(fcb, archivo); // le agrego un puntero directo
		cantidad_bloques_a_agregar --;
	}
	t_list* list_nro_de_bloques = buscar_bloques_libres(cantidad_bloques_a_agregar);
	char tamanio_str[20];
	sprintf(tamanio_str, "%d", tamanio_a_truncar);
	config_set_value(archivo,"TAMANIO_ARCHIVO",tamanio_str);

	for(int i = 0; i < cantidad_bloques_a_agregar; i++){
			uint32_t bloque = *(uint32_t*)list_get(list_nro_de_bloques, i);
			t_list* list_nro_de_bloques_indirecto = cant_bloques_puntero_indirecto(fcb->puntero_indirecto,  cuantos_bloques_venia_usando);
			int numero_bloques_indirecto = list_size(list_nro_de_bloques_indirecto);
			fseek(f_bloques, fcb->puntero_indirecto*superbloque->block_size + (numero_bloques_indirecto*sizeof(uint32_t)+ sizeof(uint32_t)*i), SEEK_SET); // la cuenta es asi porque cada vez que se mueve lo hace con un offset
			fwrite(&bloque, sizeof(uint32_t),1, f_bloques); //REVISAR que bloque al ser ahora puntero funcione igual el fwrite
			//logs: // AGREGAR LOGS
	}

	config_save(archivo);//llave del for
}


void reducir_tamanio_archivo(uint32_t tamanio_a_truncar, uint32_t cuantos_bloques_venia_usando, fcb_t* fcb, t_config* archivo){
					uint32_t cantidad_bloques_con_nuevo_tamanio = ceil_casero(tamanio_a_truncar,superbloque->block_size); //cantidad de bloques necesarios para direccionar el nuevo tamanio del archivo.
					uint32_t cantidad_bloques_a_deletear_neto =  cuantos_bloques_venia_usando - cantidad_bloques_con_nuevo_tamanio ; //ESTOS SON LOS NETOS SIN PUNTERO INDIRECTOS

					t_list* list_bloques = cant_bloques_puntero_indirecto(fcb->puntero_indirecto,  cuantos_bloques_venia_usando); //agarro los bloques indirectos
					uint32_t cant_bloques_en_bloque_indirecto = list_size(list_bloques);
					cant_bloques_en_bloque_indirecto = add_and_reverse(cant_bloques_en_bloque_indirecto, fcb->puntero_indirecto);
					bitarray_clean_bit(bitarray,fcb->puntero_indirecto);
					for(int i=0;i < list_size(list_bloques);i++){
						//borro la canridad de bloques del indirecto que es el bloque indirecto y seteo al bloque indirecto con -1
						if(cant_bloques_en_bloque_indirecto ==  cantidad_bloques_a_deletear_neto){
							bitarray_clean_bit(bitarray,fcb->puntero_indirecto);
						}
						if(cantidad_bloques_a_deletear_neto > 0){
							uint32_t* bloque = list_get(list_bloques,i);
							bitarray_clean_bit(bitarray,bloque); //puede ser? cambie al de arriba por este
							cantidad_bloques_a_deletear_neto --;
						}
					}
}


void generar_conexion_con_memoria(){
	fd_memoria = crear_conexion(logger,"File system",c->ip_memoria, c->puerto_memoria);
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
			       pthread_join(hilo, NULL);
			   }
		}

}

