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
	//uint32_t parametro1;
	//char* parametro2; //cambiado a char porque en send_rcvs.h esta definido asi en vez de uint32_t*

	for(i = 0; i < cant_instrucciones; i++){
		if (recv(socket_cliente, &code_instruccion, sizeof(op_code), NULL) != sizeof(op_code)) {
			log_error(logger, "Error al recibir una instruccion");
		}

		switch(code_instruccion){
			case SET:{
				uint32_t* p1;
				char* p2;

				if(!recv_SET(socket_cliente, &p1, &p2))
					log_error(logger, "Error al recibir SET");
				//agregar
				break;
			}
			case MOV_IN:{
				uint32_t* p1;
				uint32_t* p2;

				if(!recv_MOV_IN(socket_cliente, &p1, &p2))
					log_error(logger, "Error al recibir MOV_IN");
				//agregar
				break;
			}
			case MOV_OUT:{
				uint32_t* p1;
				uint32_t* p2;

				if(!recv_MOV_OUT(socket_cliente, &p1, &p2))
					log_error(logger,"Error al recibir MOV_OUT");
				//agregar
				break;
			}
			case IO:{
				uint32_t* p1;

				if(!recv_IO(socket_cliente, &p1))
					log_error(logger, "Error al recibir IO");
				//agregar
				break;
			}
			case F_OPEN:{
				char* p1;

				if(!recv_F_OPEN(socket_cliente, &p1))
					log_error(logger,"Error al recibir F_OPEN");
				break;
			}
			case F_CLOSE:{
				char* p1;

				if(!recv_F_CLOSE(socket_cliente, &p1))
					log_error(logger,"Error al recibir F_CLOSE");
				//agregar
				break;
			}
			case F_SEEK:{
				char* p1;
				uint32_t* p2;

				if(!recv_F_SEEK(socket_cliente, &p1, &p2))
					log_error(logger,"Error al recibir F_SEEK");
				//agregar
				break;
			}
			case F_READ:{
				char* p1;
				uint32_t* p2;
				uint32_t* p3;

				if(!recv_F_READ(socket_cliente, &p1, &p2, &p3))
					log_error(logger,"Error al recibir F_READ");
				//agregar
				break;
			}
			case F_WRITE:{
				char* p1;
				uint32_t* p2;
				uint32_t* p3;

				if(!recv_F_WRITE(socket_cliente, &p1, &p2, &p3))
					log_error(logger,"Error al recibir F_WRITE");
				//agregar
				break;
			case F_TRUNCATE:{
				char* p1;
				uint32_t p2;

				if(!recv_F_TRUNCATE(socket_cliente, &p1, &p2))
					log_error(logger,"Error al recibir F_TRUNCATE");
				//agregar
				break;
			}
			}
			case WAIT:{
				char* p1;

				if(!recv_WAIT(socket_cliente, &p1))
					log_error(logger, "Error al recibir WAIT");
				//agregar
				break;
			}
			case SIGNAL:{
				char* p1;

				if(!recv_SIGNAL(socket_cliente, &p1))
					log_error(logger,"Error al recibir SIGNAL");
				//agregar
				break;
			}
			case CREATE_SEGMENT:{
				uint32_t* p1;
				uint32_t* p2;

				if(!recv_CREATE_SEGMENT(socket_cliente, &p1, &p2))
					log_error(logger,"Error al recibir CREATE_SEGMENT");
				//agregar
				break;
			}
			case DELETE_SEGMENT:{
				uint32_t* p1;

				if(!recv_DELETE_SEGMENT(socket_cliente, &p1))
					log_error(logger,"Error al recibir DELETE_SEGMENT");
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

		}

	}


	if(!recv_CONTEXTO_EJECUCION(socket_cliente, contexto)){
		log_error(logger, "Error al recibir el contexto de ejecucion");
		exit(1);
	}


}


void cargar_instruccion_a_lista(instruccion una_instruccion, t_list* una_lista){

}

