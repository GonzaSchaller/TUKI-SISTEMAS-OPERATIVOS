#include "ejecutar_instrucciones.h"

void ejecutar_SET(int socket, pcb_cpu* pcb_proceso, uint32_t param1, char param2){
	pcb_proceso->PC += 1;
	switch(param1){
		case AX:{
			pcb_proceso -> registros -> AX = param2;
			break;
		}
		case BX:{
			pcb_proceso -> registros -> BX = param2;
			break;
		}
		case CX:{
			pcb_proceso -> registros -> CX = param2;
			break;
		}
		case DX:{
			pcb_proceso -> registros -> DX = param2;
			break;
		}
		case EAX:{
			pcb_proceso -> registros -> EAX = param2;
			break;
		}
		case EBX:{
			pcb_proceso -> registros -> EBX = param2;
			break;
		}
		case ECX:{
			pcb_proceso -> registros -> ECX = param2;
			break;
		}
		case EDX:{
			pcb_proceso -> registros -> EDX = param2;
			break;
		}
		case RAX:{
			pcb_proceso -> registros -> RAX = param2;
			break;
		}
		case RBX:{
			pcb_proceso -> registros -> RBX = param2;
			break;
		}
		case RCX:{
			pcb_proceso -> registros -> RCX = param2;
			break;
		}
		case RDX:{
			pcb_proceso -> registros -> RDX = param2;
			break;
		}
	}
	//send(socket, SET, sizeof(op_code), NULL);
	//uso los datos registro_cpu y ahi guardo el parametro string (los que estan en pcb)
	//mandar el char registro
}

//traducir la DL a DF
//preguntar a memoria que hay en esa DL
//guardar la info en el registro que me dan
void ejecutar_MOV_IN(){
	//cosas que tengo que mandar a Memoria: aplica para mov_out
	//PID
	//send_PID(socket_memoria,pcb_proceso->PID); //ijarse del socket de memoria.
	//traducir.
	//mandar a memoria.
	//TAMANIO
	//QUIEN SOS? OPCIONAL (maniana preguntamos).
}

void ejecutar_MOV_OUT(){

}

void ejecutar_IO(int socket, uint32_t tiempo, contexto_ejecucion* contexto_de_ejecucion){
	contexto_de_ejecucion -> PC += 1;
	send_CONTEXTO_EJECUCION(socket, contexto_de_ejecucion);
	send(socket, IO, sizeof(op_code), NULL);
	send_tiempo_bloqueante(socket, tiempo);
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


