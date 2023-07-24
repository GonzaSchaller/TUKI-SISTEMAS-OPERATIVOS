#include "comunicacion.h"
#include <send_rcvs.h>

extern t_log*log_memoria;
extern segmento_t *segmento_0;
//extern t_list*segmentos_ocupados;

uint32_t cant_procesos = 0;
t_list* procesos;
extern t_list*lista_de_pids;
extern t_config_memoria *cfg;

typedef struct{
	int fd;
	char*server_name;
} t_procesar_conexion_args;


void procesar_conexionn(void* void_args){

	t_procesar_conexion_args*args = (t_procesar_conexion_args*) void_args;
	int cliente_socket = args->fd;
	char*server_name = args->server_name;
	free(args);


	op_code cop;
		while(cliente_socket!=-1){
			if(recv(cliente_socket,&cop,sizeof(op_code),0) == 0){
				log_info(log_memoria,"DISCONECT!");
				return;
			}

		switch(cop){
			case HANDSHAKE:{
				uint32_t handshake;
				uint32_t resultOk = 0;
				//uint32_t resultError = -1;

				if(!recv_handshake(cliente_socket,&handshake)){
					log_error(log_memoria,"Fallo recibiendo el handshake");
					break;
				}
				if(handshake == 1){
					log_info(log_memoria,"conexion creada con %s",server_name);
					send_PC(cliente_socket,resultOk);
				}

				break;
			}


			case INICIAR_ESTRUCTURAS:{
				uint32_t pid;

				list_add(lista_de_pids,&pid);

				recv_PID(cliente_socket, &pid);
				log_info(log_memoria,"Creación de Proceso PID: %d",pid);

				t_list* tabla_de_segmentos = list_create();
				segmento_0->pid = pid;

				list_add(tabla_de_segmentos,(void*)segmento_0);

				send_TABLA_SEGMENTOS(cliente_socket,tabla_de_segmentos);

				break;
			}

			case CREATE_SEGMENT:{
				uint32_t id;
				uint32_t size;
				estados_segmentos estado = EXITOSO ;
				uint32_t pid;

				if(!recv_CREATE_SEGMENT(cliente_socket, & id, &size)) {
					log_error(log_memoria,"error recibiendo CREATE_SEGMENT"); break;}

				recv_PID(cliente_socket, &pid);

				if(!entra_en_memoria(size)){  //no entra ni en mp
					log_error(log_memoria,"no entra en mp");
					estado = FALLIDO;
					send(cliente_socket,&estado,sizeof(estado),0);
				}
				else if(!entra_en_hueco_mas_grande(size)){ //si entra en el hueco mas grande no hay que compactar :)
					estado = COMPACTAR;
					uint32_t confirmacion;

					send(cliente_socket,&estado,sizeof(estado),0);
					recv(cliente_socket, &confirmacion, sizeof(confirmacion), 0);

					if(confirmacion == COMPACTAR){
						log_info(log_memoria,"Inicio de compactacion");
						if(compactar_memoria()){
							usleep(cfg->RETARDO_COMPACTACION * 1000); /////////EL RETARDO
							ordenar_lista_pid_por_pid();
							uint32_t tamanio_list_pid = list_size(lista_de_pids);

							for(int i = 0;i<tamanio_list_pid;i++){
								uint32_t* pid_s = list_get(lista_de_pids,i);
								t_list*list_proceso_i = filtrar_lista_por_pid(*pid_s);

								//deberia enviarle el pid primero
								send_PID(cliente_socket, *pid_s);
								send_TABLA_SEGMENTOS(cliente_socket,list_proceso_i);

								list_add(list_proceso_i,segmento_0);
								ordenat_lista_por_ids(list_proceso_i);
								uint32_t cant_segmentos_por_proceso = list_size(list_proceso_i);

								for(int u=0;u<cant_segmentos_por_proceso;u++){
									segmento_t* segmento = list_get(list_proceso_i,u);
									log_info(log_memoria,"PID <%d> - Segmento <%d> - Base <%d> - Tamanio <%d> ",segmento->pid,segmento->id,segmento->direccion_Base,segmento->tamanio);
									log_info(log_memoria,"\n");
								}
								log_info(log_memoria,"\n");
							}//for
						}
					}} else{ //hay espacio entonces se crea.
						log_info(log_memoria,"hay espacio disponible... creando segmento. \n");
						send(cliente_socket,&estado,sizeof(estado),0);

						segmento_t* segmento = crear_segmento(id,size,pid);

						if(segmento == NULL){
							log_error(log_memoria,"algo salio mal creando el segmento ");
						}
						uint32_t base = segmento->direccion_Base;
						send_BASE_SEGMENTO(cliente_socket,base);

						}
				break;
			}

			case DELETE_SEGMENT: {
				uint32_t id;
				t_list* ts_kernel = list_create();
				uint32_t pid;

				recv_ID_SEGMENTO(cliente_socket, &id);
				recv_TABLA_SEGMENTOS(cliente_socket,&ts_kernel);
				recv_PID(cliente_socket,&pid);

				// me devuelve la tabla de ese segmento.
				t_list * tsegmentos_pid = list_create();
				tsegmentos_pid = filtrar_lista_por_pid(pid);
				uint32_t base = buscar_en_lista_por_id_devolver_base(tsegmentos_pid,id); //busco la base del id a eliminar.
				//elimino por base
				if(borrar_segmento(base,pid)) {
					log_info(log_memoria,"Eliminación de Proceso PID: <%d>", pid);
				}

				list_remove_by_condition(ts_kernel,&seg_con_id_igual);

				send_TABLA_SEGMENTOS(cliente_socket,ts_kernel);
				list_destroy_and_destroy_elements(ts_kernel, (void*) free);
				//deletear la lista. TODO

				break;
			}

			case FINALIZAR_ESTRUCTURAS:{
				uint32_t pid;
				t_list* ts ;

				recv_PID(cliente_socket, &pid);
				recv_TABLA_SEGMENTOS(cliente_socket,&ts);

				log_info(log_memoria,"Eliminación de Proceso PID: %d",pid);

				uint32_t lenght = list_size(ts);
				for(int i=1;i<lenght;i++){
					segmento_t* seg = list_get(ts, i);
					borrar_segmento(seg->direccion_Base,pid);
				}

				eliminar_pid_lista_pids(pid);


				break;
			}
			case READ: {
				char*contenido = NULL;
				uint32_t pid; //nice
				uint32_t direccion_fisica;//nice
				uint32_t tamanio;
				//extra_code estado;
                //uint32_t cop;
                pthread_mutex_lock(&mutex_read_write);
                recv_READ(cliente_socket,&direccion_fisica,&tamanio); // en caso de cpu seran tamanios de 4,8,16 bytes, en caso de filesystem no se sabe
				recv_PID(cliente_socket, &pid);
				usleep(cfg->RETARDO_MEMORIA * 1000);

                contenido = leer_contenido(direccion_fisica,tamanio);
				send_contenido_leido(cliente_socket,contenido);

				log_info(log_memoria,"PID: %d - Acción: Leer - Dirección física: %d - Tamaño: <%d> - Origen: <%s>",pid,direccion_fisica,tamanio,server_name);
				pthread_mutex_unlock(&mutex_read_write);
			}

			break;

			case WRITE:{
				uint32_t pid;
				uint32_t tamanio;
				uint32_t direccion_fisica;
				extra_code estado;
				char*contenido;

				pthread_mutex_lock(&mutex_read_write);
				recv_WRITE(cliente_socket,&direccion_fisica,&contenido);
				recv_cant_bytes(cliente_socket,&tamanio);
				recv_PID(cliente_socket, &pid);

				log_info(log_memoria,"PID: %d - Acción: Escribir - Dirección física: %d - Tamaño: <%d> - Origen: <%s>",pid,direccion_fisica,tamanio,server_name);

				usleep(cfg->RETARDO_MEMORIA * 1000);

				if(escribir_contenido((void*)contenido,direccion_fisica,tamanio)){
					estado = EXITOSO;
					send_OK_CODE(cliente_socket,estado);
				}
				pthread_mutex_unlock(&mutex_read_write);
				break;
			}//write
				}
				}//while
		log_warning(log_memoria,"cliente %s desconectado ",server_name);
		return;
}



int server_escuchar(t_log* log_memoria,char* server_name, int server_socket) {

	while(1){
		int cliente_socket = esperar_cliente(log_memoria, server_socket);

   if (cliente_socket != -1) {
       pthread_t hilo;
       t_procesar_conexion_args* args = malloc(sizeof(t_procesar_conexion_args));
       args->fd = cliente_socket;
       args->server_name = server_name;
       pthread_create(&hilo, NULL, (void*) procesar_conexionn, (void*) args);
       pthread_detach(hilo);
   	   }
	}
	return 0;

}
