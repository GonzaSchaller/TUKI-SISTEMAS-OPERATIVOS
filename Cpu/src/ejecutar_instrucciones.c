#include "ejecutar_instrucciones.h"

int socket_cliente_kernel;
extern int conexion_memoria;
extern t_log*logger;

void set_socket_kernel(int socket){
	socket_cliente_kernel = socket;
}
void ejecutar_SET(pcb_cpu* pcb_proceso, uint32_t registro, char* valor){
	switch(registro){
		case AX:{
			strcpy(pcb_proceso -> registros.AX, valor);
			//pcb_proceso -> registros.AX = param2;
			break;
		}
		case BX:{
			strcpy(pcb_proceso -> registros.BX, valor);
			//pcb_proceso -> registros.BX = param2;
			break;
		}
		case CX:{
			strcpy(pcb_proceso -> registros.CX, valor);
			//pcb_proceso -> registros.CX = param2;
			break;
		}
		case DX:{
			strcpy(pcb_proceso -> registros.DX, valor);
			//pcb_proceso -> registros.DX = param2;
			break;
		}
		case EAX:{
			strcpy(pcb_proceso -> registros.EAX, valor);
			//pcb_proceso -> registros.EAX = param2;
			break;
		}
		case EBX:{
			strcpy(pcb_proceso -> registros.EBX, valor);
			//pcb_proceso -> registros.EBX = param2;
			break;
		}
		case ECX:{
			strcpy(pcb_proceso -> registros.ECX, valor);
			//pcb_proceso -> registros.ECX = param2;
			break;
		}
		case EDX:{
			strcpy(pcb_proceso -> registros.EDX, valor);
			//pcb_proceso -> registros.EDX = param2;
			break;
		}
		case RAX:{
			strcpy(pcb_proceso -> registros.RAX, valor);
			//pcb_proceso -> registros.RAX = param2;
			break;
		}
		case RBX:{
			strcpy(pcb_proceso -> registros.RBX, valor);
			//pcb_proceso -> registros.RBX = param2;
			break;
		}
		case RCX:{
			strcpy(pcb_proceso -> registros.RCX, valor);
			//pcb_proceso -> registros.RCX = param2;
			break;
		}
		case RDX:{
			size_t max_size = sizeof(pcb_proceso->registros.RDX); // Tamaño máximo del destino
			strncpy(pcb_proceso->registros.RDX, valor, max_size);
			//pcb_proceso -> registros.RDX = param2;
			break;
		}
	}
	pcb_proceso -> PC += 1;
}

//traducir la DL a DF
//preguntar a memoria que hay en esa DL
//guardar la info en el registro que me dan

char* recibir_de_memoria(uint32_t df,uint32_t size,uint32_t pid){
	char*valor;
	if(!send_READ(conexion_memoria,df,size)){log_error(logger, "error mandando read a memoria");} // en caso de cpu seran tamanios de 4,8,16 bytes, en caso de filesystem no se sabe
	if(!send_PID(conexion_memoria, pid)){log_error(logger, "error mandando PID a memoria");}
	if(!recv_contenido_leido(conexion_memoria,&valor)){log_error(logger,"error recibiendo contenido leido de memoria");}
	return valor;
}

int ejecutar_MOV_IN(pcb_cpu* pcb_proceso, uint32_t registro, uint32_t dir_logica){
	//************************************************************* traduzco la DL
	t_list* listaSegmentos = pcb_proceso -> TSegmento;
	uint32_t dir_fisica = obtener_dir_fisica(dir_logica, listaSegmentos);

	if(dir_fisica == -1){
		//COMPLETAR caso de Segmentation Fault
		/* Lo que dice la consigna:
		"En caso de que el desplazamiento dentro del segmento (desplazamiento_segmento)
		sumado al tamaño a leer / escribir, sea mayor al tamaño del segmento, deberá devolverse
		el Contexto de Ejecución al Kernel para que este lo finalice con motivo de
		Error: Segmentation Fault (SEG_FAULT)"
		 */
		contexto_ejecucion contexto_actualizado;

		contexto_actualizado.PID = pcb_proceso -> PID;
		contexto_actualizado.PC = pcb_proceso -> PC;
		contexto_actualizado.registros = pcb_proceso -> registros;
		contexto_actualizado.TSegmento = pcb_proceso ->TSegmento;

		send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_actualizado);
		send_ERROR(socket_cliente_kernel);

		//“PID: <PID> - Error SEG_FAULT- Segmento: <NUMERO SEGMENTO> - Offset: <OFFSET> - Tamaño: <TAMAÑO>” TODO
		return 1; //para que corte la ejecucion de las instrucciones (se usa en execute_decode en recibo_instrucciones.c)
	}
	else{
	char* valor;

	//TODO logs acceso a memoria
	//“PID: <PID> - Acción: <LEER / ESCRIBIR> - Segmento: <NUMERO SEGMENTO> - Dirección Física: <DIRECCION FISICA> - Valor: <VALOR LEIDO / ESCRITO>”
	
	//************************************************************* guardo en el registro:
	switch(registro){
			case AX:{
				valor = recibir_de_memoria(dir_fisica,4,pcb_proceso->PID);
				strcpy(pcb_proceso -> registros.AX, valor);
				//pcb_proceso -> registros.AX = param2;
				break;
			}
			case BX:{
				valor = recibir_de_memoria(dir_fisica,4,pcb_proceso->PID);
				strcpy(pcb_proceso -> registros.BX, valor);
				//pcb_proceso -> registros.BX = param2;
				break;
			}
			case CX:{
				valor = recibir_de_memoria(dir_fisica,4,pcb_proceso->PID);
				strcpy(pcb_proceso -> registros.CX, valor);
				//pcb_proceso -> registros.CX = param2;
				break;
			}
			case DX:{
				valor = recibir_de_memoria(dir_fisica,4,pcb_proceso->PID);
				strcpy(pcb_proceso -> registros.DX, valor);
				//pcb_proceso -> registros.DX = param2;
				break;
			}
			case EAX:{
				valor = recibir_de_memoria(dir_fisica,8,pcb_proceso->PID);
				strcpy(pcb_proceso -> registros.EAX, valor);
				//pcb_proceso -> registros.EAX = param2;
				break;
			}
			case EBX:{
				valor = recibir_de_memoria(dir_fisica,8,pcb_proceso->PID);
				strcpy(pcb_proceso -> registros.EBX, valor);
				//pcb_proceso -> registros.EBX = param2;
				break;
			}
			case ECX:{
				valor = recibir_de_memoria(dir_fisica,8,pcb_proceso->PID);
				strcpy(pcb_proceso -> registros.ECX, valor);
				//pcb_proceso -> registros.ECX = param2;
				break;
			}
			case EDX:{
				valor = recibir_de_memoria(dir_fisica,8,pcb_proceso->PID);
				strcpy(pcb_proceso -> registros.EDX, valor);
				//pcb_proceso -> registros.EDX = param2;
				break;
			}
			case RAX:{
				valor = recibir_de_memoria(dir_fisica,16,pcb_proceso->PID);
				strcpy(pcb_proceso -> registros.RAX, valor);
				//pcb_proceso -> registros.RAX = param2;
				break;
			}
			case RBX:{
				valor = recibir_de_memoria(dir_fisica,16,pcb_proceso->PID);
				strcpy(pcb_proceso -> registros.RBX, valor);
				//pcb_proceso -> registros.RBX = param2;
				break;
			}
			case RCX:{
				valor = recibir_de_memoria(dir_fisica,16,pcb_proceso->PID);
				strcpy(pcb_proceso -> registros.RCX, valor);
				//pcb_proceso -> registros.RCX = param2;
				break;
			}
			case RDX:{
				valor = recibir_de_memoria(dir_fisica,16,pcb_proceso->PID);
				strcpy(pcb_proceso -> registros.RDX, valor);
				//pcb_proceso -> registros.RDX = param2;
				break;
			}
		}
	pcb_proceso -> PC += 1;
	return 0; //para que no corte la ejecucion de las instrucciones (se usa en execute_decode en recibo_instrucciones.c)
	}
}


void enviar_a_memoria(char*valor,uint32_t df,uint32_t size,uint32_t pid){

	uint32_t estado;
	if(!send_WRITE(conexion_memoria,df,valor)){log_error(logger,"error mandando write a memoria");}
	if(!send_cant_bytes(conexion_memoria,size)){log_error(logger,"error mandando la cantidad de bytes a memoria");}
	if(!send_PID(conexion_memoria,pid)){log_error(logger,"error mandando el pid a memoria");}

	if(!recv_OK_CODE(conexion_memoria,&estado))log_error(conexion_memoria,"error recibiendo ok_code de memoria");
	if(estado == EXITOSO){
		log_info(logger,"memoria escribio correctamente");
	}
}

int ejecutar_MOV_OUT(pcb_cpu* pcb_proceso, uint32_t registro, uint32_t dir_logica){
	
	//************************************************************* traduzco la DL
	t_list* listaSegmentos = pcb_proceso -> TSegmento;
	uint32_t dir_fisica = obtener_dir_fisica(dir_logica, listaSegmentos);

	if(dir_fisica == -1){
		//COMPLETAR caso de Segmentation Fault
		/* Lo que dice la consigna:
		"En caso de que el desplazamiento dentro del segmento (desplazamiento_segmento)
		sumado al tamaño a leer / escribir, sea mayor al tamaño del segmento, deberá devolverse
		el Contexto de Ejecución al Kernel para que este lo finalice con motivo de
		Error: Segmentation Fault (SEG_FAULT)"
		 */
		contexto_ejecucion contexto_actualizado;

		contexto_actualizado.PID = pcb_proceso -> PID;
		contexto_actualizado.PC = pcb_proceso -> PC;
		contexto_actualizado.registros = pcb_proceso -> registros;
		contexto_actualizado.TSegmento = pcb_proceso ->TSegmento;

		send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_actualizado);
		send_ERROR(socket_cliente_kernel);

		//log_info(logger,"PID: <%d> - Error SEG_FAULT- Segmento: <%> - Offset: <%d> - Tamaño: <%d>",pcb_proceso->PID,);
		return 1; //corta ejecucion de ejecucion (se usa en execute_decode en recibo_instrucciones.c)
	}
	else{
	char* valor;
	
	//TODO logs acceso a memoria
	//“PID: <PID> - Acción: <LEER / ESCRIBIR> - Segmento: <NUMERO SEGMENTO> - Dirección Física: <DIRECCION FISICA> - Valor: <VALOR LEIDO / ESCRITO>”
	
	//obtengo el dato del registro
	switch(registro){
		case AX:{
			strcpy(valor, pcb_proceso -> registros.AX);
			enviar_a_memoria(valor,dir_fisica,4,pcb_proceso->PID);
			//pcb_proceso -> registros.AX = param2;
			break;
		}
		case BX:{
			strcpy(valor, pcb_proceso -> registros.BX);
			enviar_a_memoria(valor,dir_fisica,4,pcb_proceso->PID);
			//pcb_proceso -> registros.BX = param2;
			break;
		}
		case CX:{
			strcpy(valor, pcb_proceso -> registros.CX);
			enviar_a_memoria(valor,dir_fisica,4,pcb_proceso->PID);
			//pcb_proceso -> registros.CX = param2;
			break;
		}
		case DX:{
			strcpy(valor, pcb_proceso -> registros.DX);
			enviar_a_memoria(valor,dir_fisica,4,pcb_proceso->PID);
			//pcb_proceso -> registros.DX = param2;
			break;
		}
		case EAX:{
			strcpy(valor, pcb_proceso -> registros.EAX);
			enviar_a_memoria(valor,dir_fisica,8,pcb_proceso->PID);
			//pcb_proceso -> registros.EAX = param2;
			break;
		}
		case EBX:{
			strcpy(valor, pcb_proceso -> registros.EBX);
			enviar_a_memoria(valor,dir_fisica,8,pcb_proceso->PID);
			//pcb_proceso -> registros.EBX = param2;
			break;
		}
		case ECX:{
			strcpy(valor, pcb_proceso -> registros.ECX);
			enviar_a_memoria(valor,dir_fisica,8,pcb_proceso->PID);
			//pcb_proceso -> registros.ECX = param2;
			break;
		}
		case EDX:{
			strcpy(valor, pcb_proceso -> registros.EDX);
			enviar_a_memoria(valor,dir_fisica,8,pcb_proceso->PID);
			//pcb_proceso -> registros.EDX = param2;
			break;
		}
		case RAX:{
			strcpy(valor, pcb_proceso -> registros.RAX);
			enviar_a_memoria(valor,dir_fisica,16,pcb_proceso->PID);
			//pcb_proceso -> registros.RAX = param2;
			break;
		}
		case RBX:{
			strcpy(valor, pcb_proceso -> registros.RBX);
			enviar_a_memoria(valor,dir_fisica,16,pcb_proceso->PID);
			//pcb_proceso -> registros.RBX = param2;
			break;
		}
		case RCX:{
			strcpy(valor, pcb_proceso -> registros.RCX);
			enviar_a_memoria(valor,dir_fisica,16,pcb_proceso->PID);
			//pcb_proceso -> registros.RCX = param2;
			break;
		}
		case RDX:{
			strcpy(valor, pcb_proceso -> registros.RDX);
			enviar_a_memoria(valor,dir_fisica,16,pcb_proceso->PID);
			//pcb_proceso -> registros.RDX = param2;
			break;
		}
	}
	pcb_proceso -> PC += 1;
	return 0; //no corta la ejecucion de las instrucciones (se usa en execute_decode en recibo_instrucciones.c)
	}
}

void ejecutar_IO(pcb_cpu* pcb_proceso, uint32_t tiempo){
	contexto_ejecucion contexto_para_kernel;

	pcb_proceso -> PC += 1;
	contexto_para_kernel.PID = pcb_proceso -> PID;
	contexto_para_kernel.PC = pcb_proceso -> PC;
	contexto_para_kernel.registros = pcb_proceso -> registros;
	contexto_para_kernel.TSegmento = pcb_proceso ->TSegmento;

	send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_para_kernel);
	send_IO(socket_cliente_kernel, tiempo);
}

void ejecutar_F_OPEN(pcb_cpu* pcb_proceso, char* archivo){
	contexto_ejecucion contexto_para_kernel;

	pcb_proceso -> PC += 1;
	contexto_para_kernel.PID = pcb_proceso -> PID;
	contexto_para_kernel.PC = pcb_proceso -> PC;
	contexto_para_kernel.registros = pcb_proceso -> registros;
	contexto_para_kernel.TSegmento = pcb_proceso ->TSegmento;

	send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_para_kernel);
	send_F_OPEN(socket_cliente_kernel, archivo);
}

void ejecutar_F_CLOSE(pcb_cpu* pcb_proceso, char* archivo){
	contexto_ejecucion contexto_para_kernel;

	pcb_proceso -> PC += 1;
	contexto_para_kernel.PID = pcb_proceso -> PID;
	contexto_para_kernel.PC = pcb_proceso -> PC;
	contexto_para_kernel.registros = pcb_proceso -> registros;
	contexto_para_kernel.TSegmento = pcb_proceso ->TSegmento;


	send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_para_kernel);
	send_F_CLOSE(socket_cliente_kernel, archivo);
}

void ejecutar_F_SEEK(pcb_cpu* pcb_proceso, char* archivo, uint32_t posicion){
	contexto_ejecucion contexto_para_kernel;

	pcb_proceso -> PC += 1;
	contexto_para_kernel.PID = pcb_proceso -> PID;
	contexto_para_kernel.PC = pcb_proceso -> PC;
	contexto_para_kernel.registros = pcb_proceso -> registros;
	contexto_para_kernel.TSegmento = pcb_proceso ->TSegmento;

	send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_para_kernel);
	send_F_SEEK(socket_cliente_kernel, archivo, posicion);
}

int ejecutar_F_READ(pcb_cpu* pcb_proceso, char* archivo, uint32_t dir_logica, uint32_t cant_bytes){
	t_list* listaSegmentos = pcb_proceso -> TSegmento;
	uint32_t dir_fisica = obtener_dir_fisica(dir_logica, listaSegmentos);
	//escribir en la DF de memoria lo del archivo
	if(dir_fisica == -1){
		contexto_ejecucion contexto_actualizado;

		contexto_actualizado.PID = pcb_proceso -> PID;
		contexto_actualizado.PC = pcb_proceso -> PC;
		contexto_actualizado.registros = pcb_proceso -> registros;
		contexto_actualizado.TSegmento = pcb_proceso ->TSegmento;

		send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_actualizado);
		send_ERROR(socket_cliente_kernel);

		return 1; //corta la ejecucion de las instrucciones (se usa en execute_decode en recibo_instrucciones.c)
	}else {
		contexto_ejecucion contexto_actualizado;

		pcb_proceso -> PC += 1;
		contexto_actualizado.PID = pcb_proceso -> PID;
		contexto_actualizado.PC = pcb_proceso -> PC;
		contexto_actualizado.registros = pcb_proceso -> registros;
		contexto_actualizado.TSegmento = pcb_proceso ->TSegmento;

		send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_actualizado);
		send_F_READ(socket_cliente_kernel,archivo,dir_fisica,cant_bytes);
		return 0; //no corta la ejecucion de las instrucciones (se usa en execute_decode en recibo_instrucciones.c)
	}
}

int ejecutar_F_WRITE(pcb_cpu* pcb_proceso, char* archivo, uint32_t dir_logica, uint32_t cant_bytes){
	t_list* listaSegmentos = pcb_proceso -> TSegmento;
	uint32_t dir_fisica = obtener_dir_fisica(dir_logica, listaSegmentos);
	if(dir_fisica == -1){
		contexto_ejecucion contexto_actualizado;

		contexto_actualizado.PID = pcb_proceso -> PID;
		contexto_actualizado.PC = pcb_proceso -> PC;
		contexto_actualizado.registros = pcb_proceso -> registros;
		contexto_actualizado.TSegmento = pcb_proceso ->TSegmento;

		send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_actualizado);
		send_ERROR(socket_cliente_kernel);
		return 1; //corta la ejecucion de las instrucciones (se usa en execute_decode en recibo_instrucciones.c)
	}else{
		contexto_ejecucion contexto_actualizado;
		pcb_proceso -> PC += 1;

		contexto_actualizado.PID = pcb_proceso -> PID;
		contexto_actualizado.PC = pcb_proceso -> PC;
		contexto_actualizado.registros = pcb_proceso -> registros;
		contexto_actualizado.TSegmento = pcb_proceso ->TSegmento;

		send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_actualizado);
		send_F_WRITE(socket_cliente_kernel,archivo,dir_fisica,cant_bytes);
		return 0; //no corta la ejecucion de las instrucciones (se usa en execute_decode en recibo_instrucciones.c)
	}

}

void ejecutar_F_TRUNCATE(pcb_cpu* pcb_proceso, char* archivo, uint32_t tamanio){
	contexto_ejecucion contexto_actualizado;
	pcb_proceso -> PC += 1;

	contexto_actualizado.PID = pcb_proceso -> PID;
	contexto_actualizado.PC = pcb_proceso -> PC;
	contexto_actualizado.registros = pcb_proceso -> registros;
	contexto_actualizado.TSegmento = pcb_proceso ->TSegmento;

	send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_actualizado);
	send_F_TRUNCATE(socket_cliente_kernel,archivo,tamanio);
}

void ejecutar_WAIT(pcb_cpu* pcb_proceso , char* recurso){
	contexto_ejecucion contexto_actualizado;
	pcb_proceso -> PC += 1;

	contexto_actualizado.PID = pcb_proceso -> PID;
	contexto_actualizado.PC = pcb_proceso -> PC;
	contexto_actualizado.registros = pcb_proceso -> registros;
	contexto_actualizado.TSegmento = pcb_proceso ->TSegmento;

	send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_actualizado);
	send_WAIT(socket_cliente_kernel, recurso);
}

void ejecutar_SIGNAL(pcb_cpu* pcb_proceso , char* recurso){
	contexto_ejecucion contexto_actualizado;
	pcb_proceso -> PC += 1;

	contexto_actualizado.PID = pcb_proceso -> PID;
	contexto_actualizado.PC = pcb_proceso -> PC;
	contexto_actualizado.registros = pcb_proceso -> registros;
	contexto_actualizado.TSegmento = pcb_proceso ->TSegmento;

	send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_actualizado);
	send_SIGNAL(socket_cliente_kernel, recurso);
}

void ejecutar_CREATE_SEGMENT(pcb_cpu* pcb_proceso){ //TODO agregar parámetros
	contexto_ejecucion contexto_actualizado;
	pcb_proceso -> PC += 1;

	contexto_actualizado.PID = pcb_proceso -> PID;
	contexto_actualizado.PC = pcb_proceso -> PC;
	contexto_actualizado.registros = pcb_proceso -> registros;
	contexto_actualizado.TSegmento = pcb_proceso ->TSegmento;

	//TODO COMPLETAR send_CREATE_SEGMENT(socket_cliente_kernel,,);
}
void ejecutar_DELETE_SEGMENT(pcb_cpu* pcb_proceso){ //TODO agregar parámetros
	contexto_ejecucion contexto_actualizado;

	pcb_proceso -> PC += 1;

	contexto_actualizado.PID = pcb_proceso -> PID;
	contexto_actualizado.PC = pcb_proceso -> PC;
	contexto_actualizado.registros = pcb_proceso -> registros;
	contexto_actualizado.TSegmento = pcb_proceso ->TSegmento;

	//TODO COMPLETAR send_ID_SEGMENTO(socket_cliente_kernel,,);
}

void ejecutar_YIELD(pcb_cpu* pcb_proceso){
	contexto_ejecucion contexto_actualizado;

	pcb_proceso -> PC += 1;
	contexto_actualizado.PID = pcb_proceso -> PID;
	contexto_actualizado.PC = pcb_proceso -> PC;
	contexto_actualizado.registros = pcb_proceso -> registros;
	contexto_actualizado.TSegmento = pcb_proceso ->TSegmento;

	send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_actualizado);
	send_YIELD(socket_cliente_kernel);
}

void ejecutar_EXIT(pcb_cpu* pcb_proceso){
	contexto_ejecucion contexto_actualizado;

	contexto_actualizado.PID = pcb_proceso -> PID;
	contexto_actualizado.PC = pcb_proceso -> PC;
	contexto_actualizado.registros = pcb_proceso -> registros;
	contexto_actualizado.TSegmento = pcb_proceso ->TSegmento;

	send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_actualizado);
	send_EXIT(socket_cliente_kernel);
}
