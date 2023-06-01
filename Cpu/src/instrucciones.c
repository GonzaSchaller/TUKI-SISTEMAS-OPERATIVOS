#include "instrucciones.h"

void recibir_instrucciones(int socket_cliente, t_log* logger){
	instruccion* instruccion_recibida;
	uint32_t* cant_instrucciones;
	contexto_ejecucion* contexto;

	if(!recv_CANT_INSTRUCCIONES(socket_cliente,cant_instrucciones)){
		log_error(logger, "Error al recibir el valor que indica la cantidad de instrucciones");
		exit(1);
	}

	uint32_t cant_i= *cant_instrucciones;
	//t_list* lista_instrucciones = listcreate();
	uint32_t i = 0;


	while(i < cant_i){

	}


	if(!recv_CONTEXTO_EJECUCION(socket_cliente, contexto)){
		log_error(logger, "Error al recibir el contexto de ejecucion");
		exit(1);
	}



	if(recv_SET(socket_cliente, instruccion_recibida -> parametro1.tipo_string, instruccion_recibida -> parametro2.tipo_int)){

	}
}


void cargar_instruccion_a_lista(){

}

