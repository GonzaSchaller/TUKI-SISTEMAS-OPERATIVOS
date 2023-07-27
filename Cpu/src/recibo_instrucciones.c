#include "recibo_instrucciones.h"

t_list* lista_pcb;
uint32_t seguir_ejecutando = 1;
char* registros_a_string(uint32_t estado_int) {
    switch (estado_int) {
        case AX:
            return "AX";
        case BX:
            return "BX";
        case CX:
            return "CX";
        case DX:
            return "DX";
        case EAX:
            return "EAX";
        case EBX:
            return "EBX";
        case ECX:
            return "ECX";
        case EDX:
            return "EDX";
        case RAX:
            return "RAX";
        case RBX:
            return "RBX";
        case RCX:
            return "RCX";
        case RDX:
            return "RDX";
        default:
            return "Registro desconocido";
    }
}

void procesar_instrucciones(int socket_cliente, t_log* logger ){
	/////r////////////////////////////////////////////// RECIBIR INSTRUCCIONES /////////////////////////
	op_code code_instruccion = -10;
	t_list* lista_instrucciones = list_create();
	uint32_t cant_instrucciones;
		if(!recv_CANT_INSTRUCCIONES(socket_cliente, &cant_instrucciones)){
			log_error(logger, "Error al recibir el valor que indica la cantidad de instrucciones");
		}
	if(cant_instrucciones > 0){
		int i;
		for(i = 0; i < cant_instrucciones; i++){
			if(verificacion_recibo_code_correctamente(socket_cliente, logger, &code_instruccion))
				cargar_instruccion_a_lista(socket_cliente, code_instruccion, lista_instrucciones, logger);
		}
	}
	/////////////////////////////////////////////////////////////////// RECIBIR CONTEXTO DE EJECUCION ////////////////////
	contexto_ejecucion contexto;
	contexto.TSegmento = list_create();

	if(!recv_CONTEXTO_EJECUCION(socket_cliente, &contexto)){
		log_error(logger, "Error al recibir el contexto de ejecucion");
	}
	//log_debug(logger, "Recibo el CONTEXTO DE EJECUCION del PROCESO <%d>", contexto.PID);
	if(!recv_TABLA_SEGMENTOS(socket_cliente, &contexto.TSegmento)){
		log_error(logger, "Error al recibir la tabla de segmentos");
	}
	//log_debug(logger, "Recibo la TABLA DE SEGMENTOS del PROCESO <%d>", contexto.PID);
	
	// paso el contexto a un pcb y ese a una lista
	pcb_cpu* pcb_proceso = malloc(sizeof(pcb_cpu));
	pcb_proceso -> PID = contexto .PID;
	pcb_proceso -> PC = contexto . PC;
	pcb_proceso -> instrucciones = lista_instrucciones;
	pcb_proceso -> registros = contexto . registros;
	//pcb_proceso ->TSegmento = list_create();
	pcb_proceso->TSegmento = contexto .TSegmento;
	//segmento_t *segmento = list_get(pcb_proceso->TSegmento, 0);
	//log_error(logger, "segmento: %d %d %d %d", segmento->direccion_Base, segmento->id, segmento->pid, segmento->tamanio);
	//log_debug(logger, "Se paso el contexto de ejecucion al PCB del proceso <%d>", contexto.PID);
	while(pcb_proceso -> PC < list_size(pcb_proceso -> instrucciones)){
		instruccion* instruccion_en_execute = fetch(pcb_proceso);
		if(decode_execute(socket_cliente, pcb_proceso, instruccion_en_execute, logger))
			break;
		//free(instruccion_en_execute);
	}
	if(lista_instrucciones != NULL) list_destroy_and_destroy_elements(lista_instrucciones, free);
	if(contexto.TSegmento != NULL)list_destroy(contexto.TSegmento);
	free(pcb_proceso);

}

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
			cargar_instruccion3(F_SEEK, "F_SEEK", param1, param2, 0, lista);
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
		default:
			//return log_error(logger,"No se reconoce el cop");
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
	instruccion* proxima_instruccion;

	proxima_instruccion = list_get(un_pcb -> instrucciones, un_pcb -> PC);
	return proxima_instruccion;
}

//tipo int para usarlo en el if que corta la ejecucion de las instrucciones
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

			log_info(logger, "PID: <%d> - Ejecutando: <SET> - <%s %s>" , pcb_proceso->PID, registros_a_string(param1), param2);
			ejecutar_SET(pcb_proceso, param1, param2);
			break;
		}
		case MOV_IN:{
			uint32_t param1 = una_instruccion -> parametro1.tipo_int;
			uint32_t param2 = una_instruccion -> parametro2.tipo_int;

			log_info(logger, "PID: <%d> - Ejecutando: <MOV_IN> - <%s, %d>", pcb_proceso->PID, registros_a_string(param1) , param2);
			corta_ejecucion = ejecutar_MOV_IN(pcb_proceso, param1, param2); //TODO REVISAR que los parametros se manden en el orden correcto
			//corta_ejecucion = 0;
			break;
		}
		case MOV_OUT:{
			uint32_t param1 = una_instruccion -> parametro1.tipo_int;
			uint32_t param2 = una_instruccion -> parametro2.tipo_int; //registro

			log_info(logger, "PID: <%d> - Ejecutando: <MOV_OUT> - <%d, %s>", pcb_proceso->PID, param1, registros_a_string(param2));
			corta_ejecucion = ejecutar_MOV_OUT(pcb_proceso, param1, param2); //TODO REVISAR que los parametros se manden en el orden
			//corta_ejecucion = 0;
			break;
		}
		case IO:{
			uint32_t tiempo = una_instruccion -> parametro1.tipo_int;

			log_info(logger, "PID: <%d> - Ejecutando: <IO> - <%d>" , pcb_proceso->PID, tiempo);
			 corta_ejecucion= ejecutar_IO(pcb_proceso, tiempo);

			break;
		}
		case F_OPEN:{
			char* param1 = una_instruccion -> parametro1.tipo_string;

			log_info(logger,"PID: <%d> - Ejecutando: <F_OPEN> - <%s>" , pcb_proceso->PID, param1);
			corta_ejecucion= ejecutar_F_OPEN(pcb_proceso, param1);

			break;
		}
		case F_CLOSE:{
			char* param1 = una_instruccion -> parametro1.tipo_string;

			log_info(logger,"PID: <%d> - Ejecutando: <F_CLOSE> - <%s>" , pcb_proceso->PID, param1);
			corta_ejecucion=ejecutar_F_CLOSE(pcb_proceso, param1);

			break;
		}
		case F_SEEK:{
			char* archivo = una_instruccion -> parametro1.tipo_string;
			uint32_t posicion = una_instruccion -> parametro2.tipo_int;

			log_info(logger,"PID: <%d> - Ejecutando: <F_SEEK> - <%s %d>" , pcb_proceso->PID, archivo, posicion);
			corta_ejecucion=ejecutar_F_SEEK(pcb_proceso, archivo, posicion);


			break;
		}
		case F_READ:{
			char* archivo = una_instruccion -> parametro1.tipo_string;
			uint32_t dir_logica = una_instruccion -> parametro2.tipo_int;
			uint32_t cant_bytes = una_instruccion -> parametro3;
			
			log_info(logger,"PID: <%d> - Ejecutando: <F_READ> - <%s %d %d>" , pcb_proceso->PID, archivo, dir_logica, cant_bytes);
			corta_ejecucion = ejecutar_F_READ(pcb_proceso, archivo, dir_logica, cant_bytes);
			break;
		}
		case F_WRITE:{
			char* archivo = una_instruccion -> parametro1.tipo_string;
			uint32_t dir_logica = una_instruccion -> parametro2.tipo_int;
			uint32_t cant_bytes = una_instruccion -> parametro3;
			
			log_info(logger,"PID: <%d> - Ejecutando: <F_WRITE> - <%s %d %d>" , pcb_proceso->PID, archivo, dir_logica, cant_bytes);
			corta_ejecucion = ejecutar_F_WRITE(pcb_proceso, archivo, dir_logica, cant_bytes);
			break;
		}
		case F_TRUNCATE:{
			char* archivo = una_instruccion -> parametro1.tipo_string;
			uint32_t tamanio = una_instruccion -> parametro2.tipo_int;
			
			log_info(logger,"PID: <%d> - Ejecutando: <F_TRUNCATE> - <%s %d>" , pcb_proceso->PID, archivo, tamanio);
			corta_ejecucion=ejecutar_F_TRUNCATE(pcb_proceso, archivo, tamanio);
			
			break;
		}
		case WAIT:{
			char* param1 = una_instruccion -> parametro1.tipo_string;
			
			log_info(logger, "PID: <%d> - Ejecutando: <WAIT> - <%s>", pcb_proceso->PID, param1);
			corta_ejecucion=ejecutar_WAIT(pcb_proceso, param1);
			break;
		}
		case SIGNAL:{
			char* param1 = una_instruccion -> parametro1.tipo_string;
			
			log_info(logger, "PID: <%d> - Ejecutando: <SIGNAL> - <%s>" , pcb_proceso->PID, param1);
			corta_ejecucion=ejecutar_SIGNAL(pcb_proceso, param1);

			break;
		}
		case CREATE_SEGMENT:{
			uint32_t id = una_instruccion -> parametro1.tipo_int;
			uint32_t tamanio = una_instruccion -> parametro2.tipo_int;

			log_info(logger, "PID: <%d> - Ejecutando: <CREATE_SEGMENT> - <%d %d>" , pcb_proceso->PID, id, tamanio);
			corta_ejecucion=ejecutar_CREATE_SEGMENT(pcb_proceso, id, tamanio);

			break;
		}
		case DELETE_SEGMENT:{
			uint32_t id =  una_instruccion -> parametro1.tipo_int;

			log_info(logger, "PID: <%d> - Ejecutando: <DELETE_SEGMENT> - <%d>" , pcb_proceso->PID, id);
			corta_ejecucion=ejecutar_DELETE_SEGMENT(pcb_proceso, id);

			break;
		}
		case YIELD:{
			log_info(logger, "PID: <%d> - Ejecutando: <YIELD>" , pcb_proceso->PID);

			corta_ejecucion=ejecutar_YIELD(pcb_proceso);

			break;
		}
		case EXIT:{
			//log_info(logger, "PID: <%d> - Ejecutando: <EXIT> " , pcb_proceso->PID);
			log_info(logger, "PID: <%d> - Ejecutando: <EXIT> " , pcb_proceso->PID);
			corta_ejecucion=ejecutar_EXIT(pcb_proceso);

			break;
		}
	}


	return corta_ejecucion;
}

