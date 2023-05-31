#include "comunicacion.h"
#include <send_rcvs.h>

extern t_log*log_memoria;
//extern *t_segmento;
int clientes[3];
int tam_hueco_mas_grande;
int memoria_disponible;
segmento_t segmento_0;

//conexiones de kernel, cpu, filesystem
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
				t_list tabla_de_segmentos;
				//aniado a la tabla de segmentos el segmento 0 y lo mando.
				//el segmento 0 ya esta en memoria.

				break;
			case CREATE_SEGMENT:
				uint32_t id;
				uint32_t tamanio;

				recv_CREATE_SEGMENT(cliente_socket,id,tamanio);
				if(tamanio>memoria_disponible){
					//no hay memoria disponible...
				}else if(tam_hueco_mas_grande<tamanio){
					//hay que compactar...
				}else log_info(log_memoria,"hay espacio disponible... creando segmento.");



				break;

			case DELETE_SEGMENT:


				break;

			case FINALIZAR_ESTRUCTURAS:
				break;

			case MOV_IN:
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
*/
/*
void tirar_threads(int socket_cliente, char*server_name , int server_socket){

   pthread_t hilo;
   t_procesar_conexion_args* args = malloc(sizeof(t_procesar_conexion_args));
   args->fd = socket_cliente;
   args->server_name = server_name;
   pthread_create(&hilo, NULL, (void*) procesar_conexionn, (void*) args);
   pthread_detach(hilo);
}
/*
int esperar_clienteM(int socket_servidor)
{
	uint32_t handshake;
	uint32_t resultOk = 0;
	uint32_t resultError = -1;
	int socket_cliente = accept(socket_servidor, NULL, NULL);

	recv(socket_cliente, &handshake, sizeof(uint32_t), MSG_WAITALL);
	if(handshake == 1){
		send(socket_cliente, &resultOk, sizeof(uint32_t), NULL);
		printf("Handshake OK\n");
		printf("Se conecto CPU!\n");
		pthread_t hilo;
					        t_procesar_conexion_args* args = malloc(sizeof(t_procesar_conexion_args));
					        args->fd = clientes[i];
					        args->server_name = server_name;
					        pthread_create(&hilo, NULL, (void*) procesar_conexion, (void*) args);
					        pthread_detach(hilo);
					        log_info(log_memoria,"hilo nro: %d \n", i);

	}
	else
	{
		send(socket_cliente, &resultError, sizeof(uint32_t), NULL);
		printf("error \n");
	}

	return socket_cliente;
}



for(int i=0;i<3;i++){
	int cliente_socket = esperar_cliente(log_memoria, server_socket);

	if (cliente_socket != -1) {
	        pthread_t hilo;
	        t_procesar_conexion_args* args = malloc(sizeof(t_procesar_conexion_args));
	        args->fd = cliente_socket;
	        args->server_name = server_name;
	        pthread_create(&hilo, NULL, (void*) procesar_conexion, (void*) args);
	        pthread_detach(hilo);
	        return 1;
	    }
	    i++;
}
return 0;*/




