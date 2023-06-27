#include "recibo_instrucciones.h"

t_list* lista_pcb;

void procesar_instrucciones(int socket_cliente, t_log* logger){
	////////////////////////////////////////////////////////////////////// RECIBIR INSTRUCCIONES /////////////////////////
	op_code code_instruccion = -10;
	uint32_t cant_instrucciones;
	if(!recv_CANT_INSTRUCCIONES(socket_cliente, &cant_instrucciones)){
		log_error(logger, "Error al recibir el valor que indica la cantidad de instrucciones");
	}
	log_info(logger, "cant instruc: <%d> ",cant_instrucciones);
	t_list* lista_instrucciones = list_create();

	if(cant_instrucciones > 0){
		int i;
		for(i = 0; i < cant_instrucciones; i++){
			if(verificacion_recibo_code_correctamente(socket_cliente, logger, &code_instruccion))
				cargar_instruccion_a_lista(socket_cliente, code_instruccion, lista_instrucciones, logger);
		}
	}

	/////////////////////////////////////////////////////////////////// RECIBIR CONTEXTO DE EJECUCION ////////////////////
	contexto_ejecucion contexto;
	contexto.TSegmento = list_create(); //todo hacer los destroy al final

	//revisar que con este recv ya obtengo el contexto completo
	if(!recv_CONTEXTO_EJECUCION(socket_cliente, &contexto)){
		log_error(logger, "Error al recibir el contexto de ejecucion");
	}
//	else{
//	log_info(logger, "PID: <%d>",contexto.PID);
//	log_info(logger, "PC:<%d>", contexto.PC);
//	log_info(logger, "AX: %s", contexto.registros.AX);
//    log_info(logger, "BX: %s", contexto.registros.BX);
//	log_info(logger, "CX: %s", contexto.registros.CX);
//	log_info(logger, "DX: %s", contexto.registros.DX);
//	log_info(logger, "EA: %s", contexto.registros.EAX);
//	log_info(logger, "EBX: %s", contexto.registros.EBX);
//	log_info(logger, "ECX: %s", contexto.registros.ECX);
//	log_info(logger, "EDX: %s", contexto.registros.EDX);
//	log_info(logger, "RAX: %s", contexto.registros.RAX);
//	log_info(logger, "RBX: %s", contexto.registros.RBX);
//	log_info(logger, "RCX: %s", contexto.registros.RCX);
//	log_info(logger, "RDX: %s", contexto.registros.RDX);
//	log_info(logger, "Tabla: %d", list_size(contexto.TSegmento));
//	}
	// paso el contexto a un pcb y ese a una lista
	pcb_cpu* pcb_proceso = malloc(sizeof(pcb_cpu));
	pcb_proceso -> PID = contexto .PID;
	pcb_proceso -> PC = contexto . PC;
	pcb_proceso -> instrucciones = lista_instrucciones;
	pcb_proceso -> registros = contexto . registros;
	//pcb_proceso ->TSegmento = list_create();
	pcb_proceso->TSegmento = contexto .TSegmento;
	//list_add(lista_pcb, pcb_proceso); // creo que no haria falta porque el kernel te manda el pcb con el pc que envia el cpu anteriormente

	instruccion* instruccion_en_execute = malloc(sizeof(instruccion));

	// ¿no tendria que primero buscar el proceso en la lista_pcb y despues buscar el la intruccion?
	while(pcb_proceso -> PC < list_size(pcb_proceso -> instrucciones)){
		instruccion_en_execute = fetch(pcb_proceso);
		if(decode_execute(socket_cliente, pcb_proceso, instruccion_en_execute, logger))
			break;
		//agregar que pasa con los otros procesos cuando dejo de ejecutar uno, paso al siguiente en teoria
	}
	/*
	ver que hacer con lista_pcb.
	entiendo que deberia:
		buscar un pcb en la lista_pcb
		ejecutar hasta que tenga que parar (diferentes motivos que cortan el while)
		buscar el siguiente pcb en la lista_pcb

	a la par ir cargandose los pcb mientras se va ejecutando alguno,
	ó ¿mejor esperar a que se carguen todos los pcb?
	*/

}

//void recibir_cant_instrucciones(int socket, t_log* logger, uint32_t* cantidad){
//	if(recv(socket, &cantidad, sizeof(uint32_t), 0) != sizeof(uint32_t)){
//		log_error(logger, "Error al recibir el valor que indica la cantidad de instrucciones");
//
//	}
//}

//era bool pero me tiraba error
int verificacion_recibo_code_correctamente(int socket, t_log* logger, op_code* code){
	if (recv(socket, code, sizeof(op_code), 0) != sizeof(op_code)) {
		log_error(logger, "Error al recibir el code");
		return 0;
	}
	return 1;
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
			cargar_instruccion1(YIELD, "YIELD", 0, 0, 0, lista);
			break;
		}
		case EXIT:{
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

instruccion* fetch(pcb_cpu* un_pcb ){
	instruccion* proxima_instruccion = malloc(sizeof(instruccion));

	proxima_instruccion = list_get(un_pcb -> instrucciones, un_pcb -> PC);
	return proxima_instruccion;
}

//era bool pero me tiraba error
// ver si tengo que retornar en vez de ser void (para cortar el while)
int decode_execute(int socket, pcb_cpu* pcb_proceso, instruccion* una_instruccion, t_log* logger){
	op_code code_instruccion = una_instruccion -> id;
	set_socket_kernel(socket); //todo hay mejor manera, no hace falta una funcion pero paja
	uint32_t retardo = (uint32_t)retardo_instruccion; // quiero asignar el valor al que esta apuntando el puntero en retardo
	retardo = retardo * 1000; //paso de milisegundos a microsegundos para usar usleep()

	int corta_ejecucion = 0;

	switch (code_instruccion){
		case SET:{
			uint32_t param1 = una_instruccion -> parametro1.tipo_int;
			char* param2 = una_instruccion -> parametro2.tipo_string;

			usleep(retardo);
			log_info(logger,"Ejecutando SET");
			//log_info(logger, "PID: < %d > - Ejecutando: < %c > - < %d, %d >", pcb_proceso->PID, una_instruccion->id);
			ejecutar_SET(pcb_proceso, param1, param2);

			corta_ejecucion = 0;
			break;
		}
		case MOV_IN:{
			uint32_t param1 = una_instruccion -> parametro1.tipo_int;
			uint32_t param2 = una_instruccion -> parametro2.tipo_int;

			//log_info(logger, "PID: %d - Ejecutando: %c - %d, %d", pcb_proceso->PID, una_instruccion->id);
			//ejecutar_MOV_IN(pcb_proceso, param1, param2);
			pcb_proceso -> PC += 1; //todo lo agrego para probar
			corta_ejecucion = 0;
			break;
		}
		case MOV_OUT:{
			uint32_t param1 = una_instruccion -> parametro1.tipo_int;
			uint32_t param2 = una_instruccion -> parametro2.tipo_int; //registro

			//log_info(logger, "PID: %d - Ejecutando: %c - %d, %d", pcb_proceso->PID, una_instruccion->id);
			//ejecutar_MOV_OUT(pcb_proceso, param2, param1);
			pcb_proceso -> PC += 1;
			corta_ejecucion = 0;
			break;
		}
		case IO:{
			uint32_t tiempo = una_instruccion -> parametro1.tipo_int;

			log_info(logger, "Ejecutando IO");
			ejecutar_IO(pcb_proceso, tiempo);

			corta_ejecucion = 1;
			break;
		}
		case F_OPEN:{
			char* param1 = una_instruccion -> parametro1.tipo_string;
			pcb_proceso -> PC += 1;
			//ejecutar_F_OPEN(pcb_proceso, param1);
			corta_ejecucion = 0;
			break;
		}
		case F_CLOSE:{
			char* param1 = una_instruccion -> parametro1.tipo_string;
			pcb_proceso -> PC += 1;
			//ejecutar_F_CLOSE(pcb_proceso, param1);
			corta_ejecucion = 0;
			break;
		}
		case F_SEEK:{
			corta_ejecucion = 0;pcb_proceso -> PC += 1;
			break;
		}
		case F_READ:{
			corta_ejecucion = 0;pcb_proceso -> PC += 1;
			break;
		}
		case F_WRITE:{
			corta_ejecucion = 0;pcb_proceso -> PC += 1;
			break;
		}
		case F_TRUNCATE:{
			corta_ejecucion = 0;pcb_proceso -> PC += 1;
			break;
		}
		case WAIT:{
			char* param1 = una_instruccion -> parametro1.tipo_string;
			log_info(logger, "Ejecutando WAIT");
			ejecutar_WAIT(pcb_proceso, param1);

			corta_ejecucion = 0;
			break;
		}
		case SIGNAL:{
			char* param1 = una_instruccion -> parametro1.tipo_string;
			log_info(logger, "PID: %d - Ejecutando: %c - %d, %d", pcb_proceso->PID, una_instruccion->id);
			ejecutar_SIGNAL(pcb_proceso, param1);

			corta_ejecucion = 0;
			break;
		}
		case CREATE_SEGMENT:{
			corta_ejecucion = 0;pcb_proceso -> PC += 1;
			break;
		}
		case DELETE_SEGMENT:{
			corta_ejecucion = 0;pcb_proceso -> PC += 1;
			break;
		}
		case YIELD:{
			log_info(logger, "Ejecutando YIELD");

			ejecutar_YIELD(pcb_proceso);

			corta_ejecucion = 1;
			break;
		}
		case EXIT:{
			log_info(logger, "Ejecutando EXIT");

			ejecutar_EXIT(pcb_proceso);

			corta_ejecucion = 1;
			break;
		}
	}
	return corta_ejecucion;
}

