#include "instrucciones.h"

void recibir_instrucciones(int socket_cliente, t_log* logger){
	op_code code_instruccion = -10;
	//instruccion instruccion_recibida;


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

void cargar_instruccion_a_lista(int socket_cliente, op_code code, t_list* lista, t_log* logger){

	switch(code){
		case SET:{
			uint32_t param1;
			char* param2;

			if(!recv_SET(socket_cliente, &param1, &param2)){
				log_error(logger, "Error al recibir SET");
				break;
			}
			cargar_instruccion2(SET, "SET", param1, param2, 0, lista);
			break;
		}
		case MOV_IN:{
			uint32_t param1;
			uint32_t param2;

			if(!recv_MOV_IN(socket_cliente, &param1, &param2)){
				log_error(logger, "Error al recibir MOV_IN");
				break;
			}
			cargar_instruccion1(MOV_IN, "MOV_IN", param1, param2, 0, lista);
			break;
		}
		case MOV_OUT:{
			uint32_t param1;
			uint32_t param2;

			if(!recv_MOV_OUT(socket_cliente, &param1, &param2)){
				log_error(logger,"Error al recibir MOV_OUT");
				break;
			}
			cargar_instruccion1(MOV_OUT, "MOV_OUT", param1, param2 , 0 , lista);
			break;
		}
		case IO:{
			uint32_t param1;

			if(!recv_IO(socket_cliente, &param1)){
				log_error(logger, "Error al recibir IO");
				break;
			}
			cargar_instruccion1(IO,"IO", param1, 0, 0,lista);
			break;
		}
		case F_OPEN:{
			char* param1;

			if(!recv_F_OPEN(socket_cliente, &param1)){
				log_error(logger,"Error al recibir F_OPEN");
				break;
			}
			cargar_instruccion3(F_OPEN, "F_OPEN", param1, 0, 0, lista);
			break;
		}
		case F_CLOSE:{
			char* param1;

			if(!recv_F_CLOSE(socket_cliente, &param1)){
				log_error(logger,"Error al recibir F_CLOSE");
				break;
			}
			cargar_instruccion3(F_CLOSE, "F_CLOSE", param1, 0, 0, lista);
			break;
		}
		case F_SEEK:{
			char* param1;
			uint32_t param2;

			if(!recv_F_SEEK(socket_cliente, &param1, &param2)){
				log_error(logger,"Error al recibir F_SEEK");
				break;
			}
			//agregar
			break;
		}
		case F_READ:{
			char* param1;
			uint32_t param2;
			uint32_t param3;

			if(!recv_F_READ(socket_cliente, &param1, &param2, &param3)){
				log_error(logger,"Error al recibir F_READ");
				break;
			}
			cargar_instruccion3( F_READ, "F_READ", param1, param2, param3, lista);
			break;
		}
		case F_WRITE:{
			char* param1;
			uint32_t param2;
			uint32_t param3;

			if(!recv_F_WRITE(socket_cliente, &param1, &param2, &param3)){
				log_error(logger,"Error al recibir F_WRITE");
				break;
			}
			cargar_instruccion3( F_WRITE, "F_WRITE", param1, param2, param3, lista);
			break;
		}
		case F_TRUNCATE:{
			char* param1;
			uint32_t param2;

			if(!recv_F_TRUNCATE(socket_cliente, &param1, &param2)){
				log_error(logger,"Error al recibir F_TRUNCATE");
				break;
			}
			cargar_instruccion3( F_TRUNCATE, "F_TRUNCATE", param1, param2, 0, lista);
			break;
		}
		case WAIT:{
			char* param1;

			if(!recv_WAIT(socket_cliente, &param1)){
				log_error(logger, "Error al recibir WAIT");
				break;
			}
			cargar_instruccion3(WAIT, "WAIT", param1, 0, 0, lista);
			break;
		}
		case SIGNAL:{
			char* param1;

			if(!recv_SIGNAL(socket_cliente, &param1)){
				log_error(logger,"Error al recibir SIGNAL");
				break;
			}
			cargar_instruccion3(SIGNAL, "SIGNAL", param1, 0, 0, lista);
			break;
		}
		case CREATE_SEGMENT:{
			uint32_t param1;
			uint32_t param2;

			if(!recv_CREATE_SEGMENT(socket_cliente, &param1, &param2)){
				log_error(logger,"Error al recibir CREATE_SEGMENT");
				break;
			}
			cargar_instruccion1(CREATE_SEGMENT, "CREATE_SEGMENT", param1, param2, 0, lista);
			break;
		}
		case DELETE_SEGMENT:{
			uint32_t param1;

			if(!recv_DELETE_SEGMENT(socket_cliente, &param1)){
				log_error(logger,"Error al recibir DELETE_SEGMENT");
				break;
			}
			cargar_instruccion1(DELETE_SEGMENT, "DELETE_SEGMENT", param1, 0, 0, lista);
			break;
		}
		case YIELD:{
			if(!recv_YIELD(socket_cliente)){
				log_error(logger, "Error al recibir YIELD");
				break;
			}
			cargar_instruccion1(YIELD, "YIELD", 0, 0, 0, lista);
			break;
		}
		case EXIT:{
			if(!recv_EXIT(socket_cliente)){
				log_error(logger, "Error al recibir EXIT");
				break;
			}
			cargar_instruccion1(EXIT,"EXIT", 0, 0, 0, lista);
			break;
		}

	}

}

void cargar_instruccion1(int id, char* nombre, uint32_t parametro1, uint32_t parametro2,uint32_t parametro3,t_list* lista){

	instruccion* estructura_instrucciones = malloc(sizeof(instruccion));

	estructura_instrucciones->id = id;
	estructura_instrucciones->nombre = nombre;
	estructura_instrucciones->parametro1.tipo_int = parametro1;
	estructura_instrucciones->parametro2.tipo_int = parametro2;
	estructura_instrucciones->parametro3 = parametro3;

	list_add(lista, estructura_instrucciones);
}

void cargar_instruccion2(int id, char* nombre, uint32_t parametro1, char* parametro2,uint32_t parametro3,t_list* lista){

	instruccion* estructura_instrucciones = malloc(sizeof(instruccion));

	estructura_instrucciones->id = id;
	estructura_instrucciones->nombre = nombre;
	estructura_instrucciones->parametro1.tipo_int = parametro1;
	estructura_instrucciones->parametro2.tipo_string= parametro2;
	estructura_instrucciones->parametro3 = parametro3;

	list_add(lista, estructura_instrucciones);
}

void cargar_instruccion3(int id, char* nombre, char* parametro1, uint32_t parametro2,uint32_t parametro3,t_list* lista){

	instruccion* estructura_instrucciones = malloc(sizeof(instruccion));

	estructura_instrucciones->id = id;
	estructura_instrucciones->nombre = nombre;
	estructura_instrucciones->parametro1.tipo_string = parametro1;
	estructura_instrucciones->parametro2.tipo_int = parametro2;
	estructura_instrucciones->parametro3 = parametro3;

	list_add(lista, estructura_instrucciones);
}

