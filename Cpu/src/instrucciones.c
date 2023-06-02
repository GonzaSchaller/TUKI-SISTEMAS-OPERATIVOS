#include "instrucciones.h"

void recibir_instrucciones(int socket_cliente, t_log* logger){
	op_code code_instruccion;
	instruccion instruccion_recibida;
	uint32_t cant_instrucciones;
	contexto_ejecucion* contexto;

	if(!recv_CANT_INSTRUCCIONES(socket_cliente,&cant_instrucciones)){
		log_error(logger, "Error al recibir el valor que indica la cantidad de instrucciones");
		exit(1);
	}

	int i = 0;
	t_list* lista_instrucciones = list_create();
	uint32_t parametro1;
	char* parametro2; //cambiado a char porque en send_rcvs.h esta definido asi en vez de uint32_t*

	for(i = 0; i < cant_instrucciones; i++){
		if (recv(socket_cliente, &code_instruccion, sizeof(op_code), NULL) != sizeof(op_code)) {
			log_error(logger, "Error al recibir una instruccion");
		}

		switch(code_instruccion){
			case SET:{
				if(!recv_SET(socket_cliente, &parametro1, &parametro2))
					log_error(logger, "Error al recibir SET");
				//agregar
				break;
			}
			case YIELD:{
				if(!recv_YIELD(socket_cliente))
					log_error(logger, "Error al recibir YIELD");
				//agregar
				break;
			}
			case EXIT:{
				if(!recv_EXIT(socket_cliente))
					log_error(logger, "Error al recibir EXIT");
				//agregar
				break;
			}
			case IO:{
				if(!recv_IO(socket_cliente, &parametro1))
					log_error(logger, "Error al recibir IO");
				//agregar
				break;
			}
			case WAIT:{
				if(!recv_WAIT(socket_cliente, &parametro2))
					log_error(logger, "Error al recibir WAIT");
				//agregar
				break;
			}
			case SIGNAL:{
				if(!recv_SIGNAL(socket_cliente, &parametro2))
					log_error(logger,"Error al recibir SIGNAL");
				//agregar
				break;
			}

		}

	}


	if(!recv_CONTEXTO_EJECUCION(socket_cliente, contexto)){
		log_error(logger, "Error al recibir el contexto de ejecucion");
		exit(1);
	}


}


void cargar_instruccion_a_lista(instruccion una_instruccion, t_list* una_lista){

}

