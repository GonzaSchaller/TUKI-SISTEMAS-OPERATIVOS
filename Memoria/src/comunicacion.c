#include "comunicacion.h"
#include <send_rcvs.h>

extern t_log*log_memoria;
extern int tam_hueco_mas_grande;
extern int memoria_disponible;
extern segmento_t *segmento_0;

//conexiones de kernel, cpu, filesystem
typedef struct{
	int fd;
	char*server_name;
} t_procesar_conexion_args;

//comentario
static void procesar_conexionn(void* void_args){
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
			case HANDSHAKE:
				uint8_t handshake;
				uint8_t resultOk = 0;
				uint8_t resultError = -1;

				if(!recv_handshake(cliente_socket,&handshake)){
					log_error(log_memoria,"Fallo recibiendo el handshake");
					break;
				}
				if(handshake == 1){
							    log_info(log_memoria,"handshake exitoso");
							    send_handshake(cliente_socket,resultOk);
							}
							else{
								   log_error(log_memoria,"no t conozcoleer() capo");
								   send_handshake(cliente_socket,resultError);
								}
				break;

			case INICIAR_ESTRUCTURAS:
				uint32_t pid;
				if(recv_INICIAR_ESTRUCTURA_MEMORIA(cliente_socket)){
					recv_PID(cliente_socket, &pid);
					log_info(log_memoria,"Creación de Proceso PID: %d",pid);

				    t_list* tabla_de_segmentos = list_create();
				    segmento_0->pid = pid;

					list_add(tabla_de_segmentos,(void*)segmento_0);

					send_TABLA_SEGMENTOS(cliente_socket,tabla_de_segmentos);
				}
				else log_error("fallo recibiendo iniciar_estructuras");

				break;
			case CREATE_SEGMENT:
				uint32_t ide;
				uint32_t size;
				estados_segmentos estado;
				uint32_t pid_cts;

				if(!recv_CREATE_SEGMENT(cliente_socket, & ide, &size)) {
				log_error(log_memoria,"error recibiendo CREATE_SEGMENT"); break;}

				recv_PID(cliente_socket, &pid_cts);

				if(!entra_en_memoria(size)){
					estado = FALLIDO;
					send(cliente_socket,&estado,sizeof(estado),0);
				}
				else if(!entra_en_hueco_mas_grande(size)){
					estado = COMPACTAR;
					uint32_t confirmacion;

					send(cliente_socket,&estado,sizeof(estado),0);
					recv(cliente_socket, &confirmacion, sizeof(confirmacion), 0);

					if(confirmacion == COMPACTAR){
					log_info(log_memoria,"Inicio de compactacion");
						//compactar_memoria(); TODO
					}
				}else{
					log_info(log_memoria,"hay espacio disponible... creando segmento.");
					send(cliente_socket,&estado,sizeof(estado),0);

					segmento_t* segmento = crear_segmento(ide,size,pid_cts);

					if(segmento == NULL){
					   log_error(log_memoria,"algo salio mal creando el segmento ");
					}
					uint32_t base = segmento->direccion_Base;
					send_BASE_SEGMENTO(cliente_socket,base);
				}
				break;

			case DELETE_SEGMENT:
				uint32_t id;
				t_list* ts_kernel;
				uint32_t pid_dlt;

				recv_TABLA_SEGMENTOS(cliente_socket,&ts_kernel);
			    recv_ID_SEGMENTO(cliente_socket, &id);
			    recv_PID(cliente_socket,&pid_dlt);

			    // me devuelve la tabla de ese segmento.
			    t_list * tsegmentos_pid = create_list_seg_by_pid(pid_dlt);
			    uint32_t base = find_id(tsegmentos_pid,id); //busco la base del id a eliminar.
			    //elimino por base
				if(borrar_segmento(base)) {
					log_info(log_memoria,"eliminacion ok");
				}

				list_remove_by_condition(ts_kernel,(void*) &seg_con_id_igual);

				send_TABLA_SEGMENTOS(cliente_socket,ts_kernel);
				//deletear la lista. TODO


				break;

			case FINALIZAR_ESTRUCTURAS:
				uint32_t pid_fe;
				t_list* ts;

				recv_PID(cliente_socket, &pid_fe);
				recv_TABLA_SEGMENTOS(cliente_socket,&ts);

				log_info(log_memoria,"Eliminación de Proceso PID: %d",&pid_fe);

				uint32_t lenght = list_size(ts);
				for(int i=0;i<lenght;i++){
					 segmento_t* seg = list_get(ts, i);
					 borrar_segmento(seg->direccion_Base);
				}

				break;

			case MOV_IN: //cambiar nombre. TODO
				//recibo la direccion fisica
				//leo lo que hay

			break;

			case MOV_OUT://cambair nombre TODO
				break;

			case COMPACTAR_MEMORIA:
				//compactar_memoria();
				break;


             }//break
       }//while
		log_warning(log_memoria,"cliente desconectado");
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

//acepto 3 clientes


/*
int server_escuchar3(t_log* log_memoria,char* server_name, int server_socket) {
	int i=0;
	while(i<3){
		int cliente_fd=esperar_cliente(log_memoria,server_socket);
		if(cliente_fd!=-1){
			 pthread_t hilo;
			        t_procesar_conexion_args* args = malloc(sizeof(t_procesar_conexion_args));
			        args->fd = cliente_fd;
			        args->server_name = server_name;
			        pthread_create(&hilo, NULL, (void*) procesar_conexionn, (void*) args);
			        pthread_detach(hilo);
			        return 1;
		}
	i++;
	}
   return 1;
}
*/



