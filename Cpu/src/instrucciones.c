#include "instrucciones.h"

void recibir_instrucciones(int socket_cliente, t_log* logger){
	op_code code_instruccion;
	instruccion instruccion_recibida;


	uint32_t cant_instrucciones = recibir_cant_instrucciones(socket_cliente, logger);
	t_list* lista_instrucciones = list_create();

	if(cant_instrucciones > 0){
		int i;
		for(i = 0; i < cant_instrucciones; i++){
			if(verificacion_recibo_code_correctamente(socket_cliente, logger, code_instruccion))
				cargar_instruccion_a_lista(socket_cliente, code_instruccion, lista_instrucciones, logger);
		}
	}



	contexto_ejecucion* contexto;

	if(!recv_CONTEXTO_EJECUCION(socket_cliente, contexto)){
		log_error(logger, "Error al recibir el contexto de ejecucion");
		exit(1);
	}


}

uint32_t recibir_cant_instrucciones(int socket, t_log* logger){
	uint32_t cantidad;
	if(!recv_CANT_INSTRUCCIONES(socket, &cantidad)){
		log_error(logger, "Error al recibir el valor que indica la cantidad de instrucciones");
		return -1;
	}
	return cantidad;
}

bool verificacion_recibo_code_correctamente(int socket, t_log* logger, op_code code){
	if (recv(socket, &code, sizeof(op_code), 0) != sizeof(op_code)) {
		log_error(logger, "Error al recibir el code");
		return false;
	}
	return true;
}

void cargar_instruccion_a_lista(int socket_cliente, op_code code, t_list* una_lista, t_log* logger){

	switch(code){
		case SET:{
			uint32_t p1;
			char* p2;

			if(!recv_SET(socket_cliente, &p1, &p2)){
				log_error(logger, "Error al recibir SET");
				break;
			}
			//agregar
			break;
		}
		case MOV_IN:{
			uint32_t p1;
			uint32_t p2;

			if(!recv_MOV_IN(socket_cliente, &p1, &p2)){
				log_error(logger, "Error al recibir MOV_IN");
				break;
			}
			//agregar
			break;
		}
		case MOV_OUT:{
			uint32_t p1;
			uint32_t p2;

			if(!recv_MOV_OUT(socket_cliente, &p1, &p2)){
				log_error(logger,"Error al recibir MOV_OUT");
				break;
			}
			//agregar
			break;
		}
		case IO:{
			uint32_t p1;

			if(!recv_IO(socket_cliente, &p1)){
				log_error(logger, "Error al recibir IO");
				break;
			}
			//agregar
			break;
		}
		case F_OPEN:{
			char* p1;

			if(!recv_F_OPEN(socket_cliente, &p1)){
				log_error(logger,"Error al recibir F_OPEN");
				break;
			}
			//agregar
			break;
		}
		case F_CLOSE:{
			char* p1;

			if(!recv_F_CLOSE(socket_cliente, &p1)){
				log_error(logger,"Error al recibir F_CLOSE");
				break;
			}
			//agregar
			break;
		}
		case F_SEEK:{
			char* p1;
			uint32_t p2;

			if(!recv_F_SEEK(socket_cliente, &p1, &p2)){
				log_error(logger,"Error al recibir F_SEEK");
				break;
			}
			//agregar
			break;
		}
		case F_READ:{
			char* p1;
			uint32_t p2;
			uint32_t p3;

			if(!recv_F_READ(socket_cliente, &p1, &p2, &p3)){
				log_error(logger,"Error al recibir F_READ");
				break;
			}
			//agregar
			break;
		}
		case F_WRITE:{
			char* p1;
			uint32_t p2;
			uint32_t p3;

			if(!recv_F_WRITE(socket_cliente, &p1, &p2, &p3)){
				log_error(logger,"Error al recibir F_WRITE");
				break;
			}
			//agregar
			break;
		}
		case F_TRUNCATE:{
			char* p1;
			uint32_t p2;

			if(!recv_F_TRUNCATE(socket_cliente, &p1, &p2)){
				log_error(logger,"Error al recibir F_TRUNCATE");
				break;
			}
			//agregar
			break;
		}
		case WAIT:{
			char* p1;

			if(!recv_WAIT(socket_cliente, &p1)){
				log_error(logger, "Error al recibir WAIT");
				break;
			}
			//agregar
			break;
		}
		case SIGNAL:{
			char* p1;

			if(!recv_SIGNAL(socket_cliente, &p1)){
				log_error(logger,"Error al recibir SIGNAL");
				break;
			}
			//agregar
			break;
		}
		case CREATE_SEGMENT:{
			uint32_t p1;
			uint32_t p2;

			if(!recv_CREATE_SEGMENT(socket_cliente, &p1, &p2)){
				log_error(logger,"Error al recibir CREATE_SEGMENT");
				break;
			}
			//agregar
			break;
		}
		case DELETE_SEGMENT:{
			uint32_t p1;

			if(!recv_DELETE_SEGMENT(socket_cliente, &p1)){
				log_error(logger,"Error al recibir DELETE_SEGMENT");
				break;
			}
			//agregar
			break;
		}
		case YIELD:{
			if(!recv_YIELD(socket_cliente)){
				log_error(logger, "Error al recibir YIELD");
				break;
			}
			//agregar
			break;
		}
		case EXIT:{
			if(!recv_EXIT(socket_cliente)){
				log_error(logger, "Error al recibir EXIT");
				break;
			}
			//agregar
			break;
		}

	}

}

