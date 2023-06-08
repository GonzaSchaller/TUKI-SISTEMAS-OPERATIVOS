#include "comunicacion.h"
#include <send_rcvs.h>

extern t_log*log_memoria;
//extern *t_segmento;
int clientes[3];
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
							    send_handshake(cliente_socket,&resultOk);
							}
							else{
								   log_error(log_memoria,"no t conozco capo");
								   send_handshake(cliente_socket,&resultError);
								}
				break;

			case INICIAR_ESTRUCTURAS: // cuadno se conecta el kernel le mando la tabla de segmentos inicial.
				//crear_tabla_segmentos(); //METERLE EL SEGMENTO 0 Y MANDARLO.
				t_list* tabla_de_segmentos = list_create();
				list_add(tabla_de_segmentos,(void*)segmento_0);
				//mandar TODO

				break;
			case CREATE_SEGMENT:
				uint32_t ide;
				uint32_t size;
				bool entra;

				if(!recv_CREATE_SEGMENT(cliente_socket, & ide, &size)) {
					log_error(log_memoria,"error recibiendo CREATE_SEGMENT"); break;}
				log_info(log_memoria,"Creación de Proceso PID: %d \n", ide);

				if(!entra_en_memoria(size)){
					//no entra el segmento ni aunque compactes send error TODO FALLIDO
				} else if(!entra_en_hueco_mas_grande()){
					//hay que compactar...send(mandarqyehayquecompactar)TODO COMPACTAR
				}else log_info(log_memoria,"hay espacio disponible... creando segmento.");

				segmento_t* segmento;

				if(crear_segmento(ide,size)==NULL){
					log_error(log_memoria,"algo salio mal creando el segmento ");
					//send que salio mal.
				}
			     //send el segmento. EXITOSO. le mando la direccion del segmento.

				break;

			case DELETE_SEGMENT:
				uint32_t id;
				recv_DELETE_SEGMENT(cliente_socket,&id);
				//recv_tabla_de_segmentos(cliente_socket,) //TODO

				if(borrar_segmento(id)) {
					log_info("eliminacion ok");
					//send que pudo eliminar. //TODO EXITOSO
				}
				t_list* tabla_de_segmentos;
				//recibo la tabla de segmentos. //TODO
				actualizar_tabla_kernel(tabla_de_segmentos);
				//send tabla actualizada //TODO

				break;

			case FINALIZAR_ESTRUCTURAS:
				//TODO
				//kernel me manda su tabla de segmentos.
				if(finalizar_estructuras()){
					log_info("Todo ok finalizando estructuras");
				}

				break;

			case MOV_IN:
				//recibo un registro

			break;

			case MOV_OUT:
				break;

			case COMPACTAR_MEMORIA:
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




