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

			case INICIAR_ESTRUCTURAS: // cuadno se conecta el kernel le mando la tabla de segmentos inicial.
				uint32_t pid;
				if(recv_INICIAR_ESTRUCTURA_MEMORIA(cliente_socket)){
					recv_PID(cliente_socket, &pid);
					log_info(log_memoria,"Creación de Proceso PID: %d",pid);

				    t_list* tabla_de_segmentos = list_create();

					list_add(tabla_de_segmentos,(void*)segmento_0);

					send_TABLA_SEGMENTOS(cliente_socket,tabla_de_segmentos);
				}
				else log_error("fallo recibiendo iniciar_estructuras");

				break;
			case CREATE_SEGMENT:
				uint32_t ide;
				uint32_t size;
				estados_segmentos estado;
				uint32_t pid3;

				if(!recv_CREATE_SEGMENT(cliente_socket, & ide, &size)) {
				log_error(log_memoria,"error recibiendo CREATE_SEGMENT"); break;}

				recv_PID(cliente_socket, &pid3);

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
					uint32_t pid4;
					log_info(log_memoria,"hay espacio disponible... creando segmento.");
					send(cliente_socket,&estado,sizeof(estado),0);

					recv_PID(cliente_socket, &pid4);
					segmento_t* segmento = crear_segmento(ide,size,pid4);

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

			    recv_ID_SEGMENTO(cliente_socket, &id);
			    recv_TABLA_SEGMENTOS(cliente_socket,&ts_kernel);

				//recv_tabla_de_segmentos(cliente_socket,) //TODO

				if(borrar_segmento(id)) {
					log_info(log_memoria,"eliminacion ok");
					//send que pudo eliminar. //TODO EXITOSO
				}

				//recibo la tabla de segmentos. //TODO
				list_remove_by_condition(ts_kernel,(void*) &seg_con_id);

				send_TABLA_SEGMENTOS(cliente_socket,ts_kernel);
				//send tabla actualizada //TODO

				break;

			case FINALIZAR_ESTRUCTURAS:
				uint32_t pid2;
				t_list* ts;

				recv_PID(cliente_socket, &pid2);
				log_info(log_memoria,"Eliminación de Proceso PID: %d",&pid2);
				//kernel me manda su tabla de segmentos.
				recv_TABLA_SEGMENTOS(cliente_socket,&ts);

				uint32_t lenght = list_size((void*) ts); //TODO fijarse si esta bien el casteo.

				for(int i=0;i<lenght;i++){
					 segmento_t* seg = list_get(ts, i);
					 borrar_segmento(seg->id);
				}

				break;

			case MOV_IN:


			break;

			case MOV_OUT:
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



