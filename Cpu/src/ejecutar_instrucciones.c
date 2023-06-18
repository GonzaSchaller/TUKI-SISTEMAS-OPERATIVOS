#include "ejecutar_instrucciones.h"

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
			strcpy(pcb_proceso -> registros.RDX, valor);
			//pcb_proceso -> registros.RDX = param2;
			break;
		}
	}
	pcb_proceso -> PC += 1;
}

//traducir la DL a DF
//preguntar a memoria que hay en esa DL
//guardar la info en el registro que me dan
void ejecutar_MOV_IN(pcb_cpu* pcb_proceso, uint32_t registro, uint32_t dir_logica){
	//cosas que tengo que mandar a Memoria: aplica para mov_out
	//PID
	//send_PID(socket_memoria,pcb_proceso->PID); //ijarse del socket de memoria.
	//traducir.
	//mandar a memoria.
	//TAMANIO
	//QUIEN SOS? OPCIONAL (maniana preguntamos).

	//************************************************************* traduzco la DL
	uint32_t tam_segmento; //ver como conseguir tamanio segmento
	uint32_t dir_fisica = obtener_dir_fisica(dir_logica, tam_segmento);

	// pedir a memoria que me pase y lo guardo en valor
	char* valor;

	//guardo en el registro:
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
				strcpy(pcb_proceso -> registros.RDX, valor);
				//pcb_proceso -> registros.RDX = param2;
				break;
			}
		}
	pcb_proceso -> PC += 1;
}

void ejecutar_MOV_OUT(pcb_cpu* pcb_proceso, uint32_t registro, uint32_t dir_logica){
	char* valor;
	//obtengo el dato del registro
	switch(registro){
		case AX:{
			strcpy(valor, pcb_proceso -> registros.AX);
			//pcb_proceso -> registros.AX = param2;
			break;
		}
		case BX:{
			strcpy(valor, pcb_proceso -> registros.BX);
			//pcb_proceso -> registros.BX = param2;
			break;
		}
		case CX:{
			strcpy(valor, pcb_proceso -> registros.CX);
			//pcb_proceso -> registros.CX = param2;
			break;
		}
		case DX:{
			strcpy(valor, pcb_proceso -> registros.DX);
			//pcb_proceso -> registros.DX = param2;
			break;
		}
		case EAX:{
			strcpy(valor, pcb_proceso -> registros.EAX);
			//pcb_proceso -> registros.EAX = param2;
			break;
		}
		case EBX:{
			strcpy(valor, pcb_proceso -> registros.EBX);
			//pcb_proceso -> registros.EBX = param2;
			break;
		}
		case ECX:{
			strcpy(valor, pcb_proceso -> registros.ECX);
			//pcb_proceso -> registros.ECX = param2;
			break;
		}
		case EDX:{
			strcpy(valor, pcb_proceso -> registros.EDX);
			//pcb_proceso -> registros.EDX = param2;
			break;
		}
		case RAX:{
			strcpy(valor, pcb_proceso -> registros.RAX);
			//pcb_proceso -> registros.RAX = param2;
			break;
		}
		case RBX:{
			strcpy(valor, pcb_proceso -> registros.RBX);
			//pcb_proceso -> registros.RBX = param2;
			break;
		}
		case RCX:{
			strcpy(valor, pcb_proceso -> registros.RCX);
			//pcb_proceso -> registros.RCX = param2;
			break;
		}
		case RDX:{
			strcpy(valor, pcb_proceso -> registros.RDX);
			//pcb_proceso -> registros.RDX = param2;
			break;
		}
	}
	//se lo mando a memoria para guardarlo en la DF
	// paso la DF y el valor
	pcb_proceso -> PC += 1;
}

void ejecutar_IO(pcb_cpu* pcb_proceso, uint32_t tiempo){
	contexto_ejecucion contexto_para_kernel;

	pcb_proceso -> PC += 1;
	contexto_para_kernel -> PID = pcb_proceso -> PID;
	contexto_para_kernel -> PC = pcb_proceso -> PC;
	contexto_para_kernel -> registros = pcb_proceso -> registros;
	contexto_para_kernel -> TSegmento = NULL; /* no manda nada, en kernel no van a pasar este dato
	actualizado porque es el mismo que me mandan en su momento*/

	send(socket_cliente_kernel, IO, sizeof(op_code), NULL);
	send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_para_kernel); //agregar el socket
	send_tiempo_bloqueante(socket_cliente_kernel, tiempo);
}

void ejecutar_F_OPEN(){}

void ejecutar_F_CLOSE(){}

void ejecutar_F_READ(){}

void ejecutar_F_WRITE(){}

void ejecutar_F_TRUNCATE(){}

void ejecutar_WAIT(int socket, contexto_ejecucion contexto_de_ejecucion, char* recurso){
	contexto_de_ejecucion -> PC += 1;
	send(socket, WAIT, sizeof(op_code), NULL);
	send_WAIT(socket, recurso);
}

void ejecutar_SIGNAL(int socket, contexto_ejecucion contexto_de_ejecucion, char* recurso){
	send(socket, SIGNAL, sizeof(op_code), NULL);
	contexto_de_ejecucion -> PC += 1;
	send_SIGNAL(socket, recurso);
}

void ejecutar_CREATE_SEGMENT(){}

void ejecutar_DELETE_SEGMENT(){}

void ejecuctar_YIELD(int socket, pcb_cpu* pcb_proceso){
	contexto_ejecucion* contexto_de_ejecucion = pcb_proceso -> instrucciones;
	contexto_de_ejecucion -> PID = pcb_proceso -> PID;
	contexto_de_ejecucion -> PC = pcb_proceso -> PC + 1;
	contexto_de_ejecucion -> registros = pcb_proceso -> registros;

	// en teoria el contexto de ejecucion tiene los TSegmento pero el pcb_proceso no, no se de donde sacarlo
	send_CONTEXTO_EJECUCION(socket, contexto_de_ejecucion);
	send(socket, YIELD, sizeof(op_code), NULL);
}

//FALTA, esta funcion hace que no no busque la siguiente isntruccion
void ejecutar_EXIT(int socket, contexto_ejecucion contexto_de_ejecucion){
	contexto_de_ejecucion.PC += 1; //si lo pongo con la -> me salta error
	send_CONTEXTO_EJECUCION(socket, contexto_de_ejecucion);
	send(socket, EXIT, sizeof(op_code), NULL);
}

