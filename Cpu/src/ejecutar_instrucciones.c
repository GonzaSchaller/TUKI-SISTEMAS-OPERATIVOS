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
			break;
		}
		case BX:{
			strcpy(pcb_proceso -> registros.BX, valor);
			break;
		}
		case CX:{
			strcpy(pcb_proceso -> registros.CX, valor);
			break;
		}
		case DX:{
			strcpy(pcb_proceso -> registros.DX, valor);
			break;
		}
		case EAX:{
			strcpy(pcb_proceso -> registros.EAX, valor);
			break;
		}
		case EBX:{
			strcpy(pcb_proceso -> registros.EBX, valor);
			break;
		}
		case ECX:{
			strcpy(pcb_proceso -> registros.ECX, valor);
			break;
		}
		case EDX:{
			strcpy(pcb_proceso -> registros.EDX, valor);
			break;
		}
		case RAX:{
			strcpy(pcb_proceso -> registros.RAX, valor);
			break;
		}
		case RBX:{
			strcpy(pcb_proceso -> registros.RBX, valor);
			break;
		}
		case RCX:{
			strcpy(pcb_proceso -> registros.RCX, valor);
			break;
		}
		case RDX:{
			size_t max_size = sizeof(pcb_proceso->registros.RDX); // Tamaño máximo del destino
			strncpy(pcb_proceso->registros.RDX, valor, max_size);
			break;
		}
	}
	pcb_proceso -> PC += 1;
}

void segmentation_fault(pcb_cpu* pcb_proceso, uint32_t segmento, uint32_t offset, uint32_t tamanio, uint32_t SF_por_tam_valor){
    contexto_ejecucion contexto_actualizado;

    contexto_actualizado.PID = pcb_proceso -> PID;
    contexto_actualizado.PC = pcb_proceso -> PC;
    contexto_actualizado.registros = pcb_proceso -> registros;
    contexto_actualizado.TSegmento = pcb_proceso ->TSegmento;

    send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_actualizado);
	send_TABLA_SEGMENTOS(socket_cliente_kernel, contexto_actualizado.TSegmento);
    send_ERROR(socket_cliente_kernel);

    log_error(logger,"PID: <%d> - Error SEG_FAULT- Segmento: <%d> - Offset: <%d> - Tamanio: <%d>", pcb_proceso -> PID, segmento, offset, tamanio);
   // if(SF_por_tam_valor)
        //log_error(logger, "Lo que se esta intentando guardar en la memoria sobrepasa su tamanio");
}


char* recibir_de_memoria(uint32_t df,uint32_t size,uint32_t pid){
	char*valor = malloc(sizeof(registros_cpu));; ;
	if(!send_READ_CPU(conexion_memoria,df,size)){log_error(logger, "error mandando read a memoria");} // en caso de cpu seran tamanios de 4,8,16 bytes, en caso de filesystem no se sabe
	if(!send_PID(conexion_memoria, pid)){log_error(logger, "error mandando PID a memoria");}
	if(!send_LEER_CONTENIDO_CPU(conexion_memoria)){
		log_error(logger, "error mandando PID a memoria");
	}
	if(!recv_contenido_leido(conexion_memoria,&valor)){log_error(logger,"error recibiendo contenido leido de memoria");}
	return valor;
}

int ejecutar_MOV_IN(pcb_cpu* pcb_proceso, uint32_t registro, uint32_t dir_logica){
	//************************************************************* traduzco la DL
	t_list* listaSegmentos = pcb_proceso -> TSegmento;
	uint32_t segmento = -1;
	uint32_t offset = -1;
	uint32_t dir_Base = -1;
	uint32_t tamanio = -1;
	uint32_t dir_fisica = -1;
	obtener_dir_fisica(dir_logica, listaSegmentos, &segmento, &offset, &dir_Base, &tamanio, &dir_fisica);

	if(dir_fisica == -1){
		segmentation_fault(pcb_proceso, segmento, offset, tamanio, 0);
		return 1; //para que corte la ejecucion de las instrucciones (se usa en execute_decode en recibo_instrucciones.c)
	}
	else{
	char* valor = malloc(sizeof(registros_cpu));

	//“PID: <PID> - Acción: <LEER / ESCRIBIR> - Segmento: <NUMERO SEGMENTO> - Dirección Física: <DIRECCION FISICA> - Valor: <VALOR LEIDO / ESCRITO>”

	//************************************************************* guardo en el registro:
	switch(registro){
			case AX:{
				valor = recibir_de_memoria(dir_fisica,4,pcb_proceso->PID);
				strcpy(pcb_proceso -> registros.AX, valor);

				break;
			}
			case BX:{
				valor = recibir_de_memoria(dir_fisica,4,pcb_proceso->PID);
				strcpy(pcb_proceso -> registros.BX, valor);

				break;
			}
			case CX:{
				valor = recibir_de_memoria(dir_fisica,4,pcb_proceso->PID);
				strcpy(pcb_proceso -> registros.CX, valor);

				break;
			}
			case DX:{
				valor = recibir_de_memoria(dir_fisica,4,pcb_proceso->PID);
				strcpy(pcb_proceso -> registros.DX, valor);

				break;
			}
			case EAX:{
				valor = recibir_de_memoria(dir_fisica,8,pcb_proceso->PID);
				strcpy(pcb_proceso -> registros.EAX, valor);

				break;
			}
			case EBX:{
				valor = recibir_de_memoria(dir_fisica,8,pcb_proceso->PID);
				strcpy(pcb_proceso -> registros.EBX, valor);

				break;
			}
			case ECX:{
				valor = recibir_de_memoria(dir_fisica,8,pcb_proceso->PID);
				strcpy(pcb_proceso -> registros.ECX, valor);

				break;
			}
			case EDX:{
				valor = recibir_de_memoria(dir_fisica,8,pcb_proceso->PID);
				strcpy(pcb_proceso -> registros.EDX, valor);

				break;
			}
			case RAX:{
				valor = recibir_de_memoria(dir_fisica,16,pcb_proceso->PID);
				strcpy(pcb_proceso -> registros.RAX, valor);

				break;
			}
			case RBX:{
				valor = recibir_de_memoria(dir_fisica,16,pcb_proceso->PID);
				strcpy(pcb_proceso -> registros.RBX, valor);

				break;
			}
			case RCX:{
				valor = recibir_de_memoria(dir_fisica,16,pcb_proceso->PID);
				strcpy(pcb_proceso -> registros.RCX, valor);

				break;
			}
			case RDX:{
				valor = recibir_de_memoria(dir_fisica,16,pcb_proceso->PID);
				strcpy(pcb_proceso -> registros.RDX, valor);

				break;
			}
		}
		log_info(logger, "PID: <%d> - Acción: <LEER> - Segmento: <%d> - Dirección Física: <%d> - Valor: <%s>", pcb_proceso -> PID, segmento, dir_fisica, valor);
	pcb_proceso -> PC += 1;
	return 0; //para que no corte la ejecucion de las instrucciones (se usa en execute_decode en recibo_instrucciones.c)
	}
}


void enviar_a_memoria(char*valor,uint32_t df,uint32_t size,uint32_t pid){

	uint32_t estado;
	if(!send_WRITE_CPU(conexion_memoria,df,valor)){log_error(logger,"error mandando write a memoria");}
	if(!send_cant_bytes(conexion_memoria,size)){log_error(logger,"error mandando la cantidad de bytes a memoria");}
	if(!send_PID(conexion_memoria,pid)){log_error(logger,"error mandando el pid a memoria");}

	if(!recv_OK_CODE(conexion_memoria,&estado))log_error(logger,"error recibiendo ok_code de memoria");
	if(estado == EXITOSO){
		log_info(logger,"Memoria escribio correctamente");
	}
}

int ejecutar_MOV_OUT(pcb_cpu* pcb_proceso, uint32_t dir_logica, uint32_t registro){ //cambie el orden entre DL y registro, si no funciona cambiarlo otra vez
	
	//************************************************************* traduzco la DL
	t_list* listaSegmentos = pcb_proceso -> TSegmento;
	uint32_t segmento = -1;
	uint32_t offset = -1;
	uint32_t dir_Base = -1;
	uint32_t tamanio = -1;
	uint32_t dir_fisica = -1;

	obtener_dir_fisica(dir_logica, listaSegmentos, &segmento, &offset, &dir_Base, &tamanio, &dir_fisica);
	uint32_t espacio_disponible = tamanio - offset -1;
	if(dir_fisica == -1){
		segmentation_fault(pcb_proceso, segmento, offset, tamanio, 0);
		return 1; //corta ejecucion de ejecucion (se usa en execute_decode en recibo_instrucciones.c)
	}
	else{
	char* valor = malloc(sizeof(registros_cpu));
	int corta_ejecucion = 0;
	//“PID: <PID> - Acción: <LEER / ESCRIBIR> - Segmento: <NUMERO SEGMENTO> - Dirección Física: <DIRECCION FISICA> - Valor: <VALOR LEIDO / ESCRITO>”
	
	//obtengo el dato del registro
	switch(registro){
		case AX:{
			strcpy(valor, pcb_proceso -> registros.AX);

			if(4 <= espacio_disponible){
				log_info(logger, "PID: <%d> - Acción: <ESCRIBIR> - Segmento: <%d> - Dirección Física: <%d> - Valor: <%s>", pcb_proceso -> PID, segmento, dir_fisica, valor);
				enviar_a_memoria(valor,dir_fisica,4,pcb_proceso->PID);
			}
			else {
				segmentation_fault(pcb_proceso, segmento, offset, tamanio, 1);
				corta_ejecucion = 1;
			}

			break;
		}
		case BX:{
			strcpy(valor, pcb_proceso -> registros.BX);

			if(4 <= espacio_disponible){
				log_info(logger, "PID: <%d> - Acción: <ESCRIBIR> - Segmento: <%d> - Dirección Física: <%d> - Valor: <%s>", pcb_proceso -> PID, segmento, dir_fisica, valor);
				enviar_a_memoria(valor,dir_fisica,4,pcb_proceso->PID);
			}
			else {
				segmentation_fault(pcb_proceso, segmento, offset, tamanio, 1);
				corta_ejecucion = 1;
			}
			break;
		}
		case CX:{
			strcpy(valor, pcb_proceso -> registros.CX);

			if(4 <= espacio_disponible){
				log_info(logger, "PID: <%d> - Acción: <ESCRIBIR> - Segmento: <%d> - Dirección Física: <%d> - Valor: <%s>", pcb_proceso -> PID, segmento, dir_fisica, valor);
				enviar_a_memoria(valor,dir_fisica,4,pcb_proceso->PID);
			}
			else {
				segmentation_fault(pcb_proceso, segmento, offset, tamanio, 1);
				corta_ejecucion = 1;
			}

			break;
		}
		case DX:{
			strcpy(valor, pcb_proceso -> registros.DX);

			if(4 <= espacio_disponible){
				log_info(logger, "PID: <%d> - Acción: <ESCRIBIR> - Segmento: <%d> - Dirección Física: <%d> - Valor: <%s>", pcb_proceso -> PID, segmento, dir_fisica, valor);
				enviar_a_memoria(valor,dir_fisica,4,pcb_proceso->PID);
			}
			else {
				segmentation_fault(pcb_proceso, segmento, offset, tamanio, 1);
				corta_ejecucion = 1;
			}

			break;
		}
		case EAX:{
			strcpy(valor, pcb_proceso -> registros.EAX);

			if(8 <= espacio_disponible){
				log_info(logger, "PID: <%d> - Acción: <ESCRIBIR> - Segmento: <%d> - Dirección Física: <%d> - Valor: <%s>", pcb_proceso -> PID, segmento, dir_fisica, valor);
				enviar_a_memoria(valor,dir_fisica,8,pcb_proceso->PID);
			}
			else {
				segmentation_fault(pcb_proceso, segmento, offset, tamanio, 1);
				corta_ejecucion = 1;
			}

			break;
		}
		case EBX:{
			strcpy(valor, pcb_proceso -> registros.EBX);

			if(8 <= espacio_disponible){
				log_info(logger, "PID: <%d> - Acción: <ESCRIBIR> - Segmento: <%d> - Dirección Física: <%d> - Valor: <%s>", pcb_proceso -> PID, segmento, dir_fisica, valor);
				enviar_a_memoria(valor,dir_fisica,8,pcb_proceso->PID);
			}
			else {
				segmentation_fault(pcb_proceso, segmento, offset, tamanio, 1);
				corta_ejecucion = 1;
			}

			break;
		}
		case ECX:{
			strcpy(valor, pcb_proceso -> registros.ECX);

			if(8 <= espacio_disponible){
				log_info(logger, "PID: <%d> - Acción: <ESCRIBIR> - Segmento: <%d> - Dirección Física: <%d> - Valor: <%s>", pcb_proceso -> PID, segmento, dir_fisica, valor);
				enviar_a_memoria(valor,dir_fisica,8,pcb_proceso->PID);
			}
			else {
				segmentation_fault(pcb_proceso, segmento, offset, tamanio, 1);
				corta_ejecucion = 1;
			}

			break;
		}
		case EDX:{
			strcpy(valor, pcb_proceso -> registros.EDX);

			if(8 <= espacio_disponible){
				log_info(logger, "PID: <%d> - Acción: <ESCRIBIR> - Segmento: <%d> - Dirección Física: <%d> - Valor: <%s>", pcb_proceso -> PID, segmento, dir_fisica, valor);
				enviar_a_memoria(valor,dir_fisica,8,pcb_proceso->PID);
			}
			else {
				segmentation_fault(pcb_proceso, segmento, offset, tamanio, 1);
				corta_ejecucion = 1;
			}

			break;
		}
		case RAX:{
			strcpy(valor, pcb_proceso -> registros.RAX);

			if(16 <= espacio_disponible){
				log_info(logger, "PID: <%d> - Acción: <ESCRIBIR> - Segmento: <%d> - Dirección Física: <%d> - Valor: <%s>", pcb_proceso -> PID, segmento, dir_fisica, valor);
				enviar_a_memoria(valor,dir_fisica,16,pcb_proceso->PID);
			}
			else {
				segmentation_fault(pcb_proceso, segmento, offset, tamanio, 1);
				corta_ejecucion = 1;
			}

			break;
		}
		case RBX:{
			strcpy(valor, pcb_proceso -> registros.RBX);

			if(16 <= espacio_disponible){
				log_info(logger, "PID: <%d> - Acción: <ESCRIBIR> - Segmento: <%d> - Dirección Física: <%d> - Valor: <%s>", pcb_proceso -> PID, segmento, dir_fisica, valor);
				enviar_a_memoria(valor,dir_fisica,16,pcb_proceso->PID);
			}
			else {
				segmentation_fault(pcb_proceso, segmento, offset, tamanio, 1);
				corta_ejecucion = 1;
			}

			break;
		}
		case RCX:{
			strcpy(valor, pcb_proceso -> registros.RCX);

			if(16 <= espacio_disponible){
				log_info(logger, "PID: <%d> - Acción: <ESCRIBIR> - Segmento: <%d> - Dirección Física: <%d> - Valor: <%s>", pcb_proceso -> PID, segmento, dir_fisica, valor);
				enviar_a_memoria(valor,dir_fisica,16,pcb_proceso->PID);
			}
			else {
				segmentation_fault(pcb_proceso, segmento, offset, tamanio, 1);
				corta_ejecucion = 1;
			}

			break;
		}
		case RDX:{
			strcpy(valor, pcb_proceso -> registros.RDX);

			if(16 <= espacio_disponible){
				log_info(logger, "PID: <%d> - Acción: <ESCRIBIR> - Segmento: <%d> - Dirección Física: <%d> - Valor: <%s>", pcb_proceso -> PID, segmento, dir_fisica, valor);
				enviar_a_memoria(valor,dir_fisica,16,pcb_proceso->PID);
			}
			else {
				segmentation_fault(pcb_proceso, segmento, offset, tamanio, 1);
				corta_ejecucion = 1;
			}

			break;
		}
	}
	pcb_proceso -> PC += 1;
	return corta_ejecucion; //puede cortar la ejecucion de las instrucciones (se usa en execute_decode en recibo_instrucciones.c)
	}
}

int ejecutar_IO(pcb_cpu* pcb_proceso, uint32_t tiempo){
	contexto_ejecucion contexto_para_kernel;

	pcb_proceso -> PC += 1;
	contexto_para_kernel.PID = pcb_proceso -> PID;
	contexto_para_kernel.PC = pcb_proceso -> PC;
	contexto_para_kernel.registros = pcb_proceso -> registros;
	contexto_para_kernel.TSegmento = pcb_proceso ->TSegmento;

	send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_para_kernel);
	send_TABLA_SEGMENTOS(socket_cliente_kernel, contexto_para_kernel.TSegmento);
	send_IO(socket_cliente_kernel, tiempo);
	op_code cop;
	if (recv(socket_cliente_kernel, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
		//log_error(logger, "Error al recibir el code");
	}
	seguir_ejecutando=recv_seguir_ejecutando(socket_cliente_kernel);
	return 1;
}

int ejecutar_F_OPEN(pcb_cpu* pcb_proceso, char* archivo){
	contexto_ejecucion contexto_para_kernel;

	pcb_proceso -> PC += 1;
	contexto_para_kernel.PID = pcb_proceso -> PID;
	contexto_para_kernel.PC = pcb_proceso -> PC;
	contexto_para_kernel.registros = pcb_proceso -> registros;
	contexto_para_kernel.TSegmento = pcb_proceso ->TSegmento;

	send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_para_kernel);
	send_TABLA_SEGMENTOS(socket_cliente_kernel, contexto_para_kernel.TSegmento);
	send_F_OPEN(socket_cliente_kernel, archivo);
	op_code cop;
	if (recv(socket_cliente_kernel, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
		//log_error(logger, "Error al recibir el code");
	}
	seguir_ejecutando=recv_seguir_ejecutando(socket_cliente_kernel);
	return seguir_ejecutando;
}

int ejecutar_F_CLOSE(pcb_cpu* pcb_proceso, char* archivo){
	contexto_ejecucion contexto_para_kernel;

	pcb_proceso -> PC += 1;
	contexto_para_kernel.PID = pcb_proceso -> PID;
	contexto_para_kernel.PC = pcb_proceso -> PC;
	contexto_para_kernel.registros = pcb_proceso -> registros;
	contexto_para_kernel.TSegmento = pcb_proceso ->TSegmento;


	send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_para_kernel);
	send_TABLA_SEGMENTOS(socket_cliente_kernel, contexto_para_kernel.TSegmento);
	send_F_CLOSE(socket_cliente_kernel, archivo);
	op_code cop;
	if (recv(socket_cliente_kernel, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
		//log_error(logger, "Error al recibir el code");
	}
	seguir_ejecutando=recv_seguir_ejecutando(socket_cliente_kernel);
	return seguir_ejecutando;
}

int ejecutar_F_SEEK(pcb_cpu* pcb_proceso, char* archivo, uint32_t posicion){
	contexto_ejecucion contexto_para_kernel;

	pcb_proceso -> PC += 1;
	contexto_para_kernel.PID = pcb_proceso -> PID;
	contexto_para_kernel.PC = pcb_proceso -> PC;
	contexto_para_kernel.registros = pcb_proceso -> registros;
	contexto_para_kernel.TSegmento = pcb_proceso ->TSegmento;

	send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_para_kernel);
	send_TABLA_SEGMENTOS(socket_cliente_kernel, contexto_para_kernel.TSegmento);
	send_F_SEEK(socket_cliente_kernel, archivo, posicion);
	op_code cop;
	if (recv(socket_cliente_kernel, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
		//log_error(logger, "Error al recibir el code");
	}
	seguir_ejecutando=recv_seguir_ejecutando(socket_cliente_kernel);
	return seguir_ejecutando;
}

int ejecutar_F_READ(pcb_cpu* pcb_proceso, char* archivo, uint32_t dir_logica, uint32_t cant_bytes){
	t_list* listaSegmentos = pcb_proceso -> TSegmento;
	uint32_t segmento = -1;
	uint32_t offset = -1;
	uint32_t dir_Base = -1;
	uint32_t tamanio = -1;
	uint32_t dir_fisica = -1;
	obtener_dir_fisica(dir_logica, listaSegmentos, &segmento, &offset, &dir_Base, &tamanio, &dir_fisica);
	//escribir en la DF de memoria lo del archivo
	if(dir_fisica == -1){
		contexto_ejecucion contexto_actualizado;

		contexto_actualizado.PID = pcb_proceso -> PID;
		contexto_actualizado.PC = pcb_proceso -> PC;
		contexto_actualizado.registros = pcb_proceso -> registros;
		contexto_actualizado.TSegmento = pcb_proceso ->TSegmento;

		send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_actualizado);
		send_TABLA_SEGMENTOS(socket_cliente_kernel, contexto_actualizado.TSegmento);
		send_ERROR(socket_cliente_kernel);
		log_error(logger,"PID: <%d> - Error SEG_FAULT- Segmento: <%d> - Offset: <%d> - Tamanio: <%d>", pcb_proceso -> PID, segmento, offset, tamanio);

		return 1; //corta la ejecucion de las instrucciones (se usa en execute_decode en recibo_instrucciones.c)
	}else {
		contexto_ejecucion contexto_actualizado;

		pcb_proceso -> PC += 1;
		contexto_actualizado.PID = pcb_proceso -> PID;
		contexto_actualizado.PC = pcb_proceso -> PC;
		contexto_actualizado.registros = pcb_proceso -> registros;
		contexto_actualizado.TSegmento = pcb_proceso ->TSegmento;

		send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_actualizado);
		send_TABLA_SEGMENTOS(socket_cliente_kernel, contexto_actualizado.TSegmento);
		send_F_READ(socket_cliente_kernel,archivo,dir_fisica,cant_bytes);
		op_code cop;
		if (recv(socket_cliente_kernel, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
		//log_error(logger, "Error al recibir el code");
		}
		seguir_ejecutando=recv_seguir_ejecutando(socket_cliente_kernel);
		return seguir_ejecutando; //no corta la ejecucion de las instrucciones (se usa en execute_decode en recibo_instrucciones.c)
	}
}

int ejecutar_F_WRITE(pcb_cpu* pcb_proceso, char* archivo, uint32_t dir_logica, uint32_t cant_bytes){
	t_list* listaSegmentos = pcb_proceso -> TSegmento;
	uint32_t segmento = -1;
	uint32_t offset = -1;
	uint32_t dir_Base = -1;
	uint32_t tamanio = -1;
	uint32_t dir_fisica = -1;
	obtener_dir_fisica(dir_logica, listaSegmentos, &segmento, &offset, &dir_Base, &tamanio, &dir_fisica);

	if(dir_fisica == -1){
		contexto_ejecucion contexto_actualizado;

		contexto_actualizado.PID = pcb_proceso -> PID;
		contexto_actualizado.PC = pcb_proceso -> PC;
		contexto_actualizado.registros = pcb_proceso -> registros;
		contexto_actualizado.TSegmento = pcb_proceso ->TSegmento;

		send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_actualizado);
		send_TABLA_SEGMENTOS(socket_cliente_kernel, contexto_actualizado.TSegmento);
		send_ERROR(socket_cliente_kernel);
		log_error(logger,"PID: <%d> - Error SEG_FAULT- Segmento: <%d> - Offset: <%d> - Tamanio: <%d>", pcb_proceso -> PID, segmento, offset, tamanio);

		return 1; //corta la ejecucion de las instrucciones (se usa en execute_decode en recibo_instrucciones.c)
	}else{
		contexto_ejecucion contexto_actualizado;
		pcb_proceso -> PC += 1;

		contexto_actualizado.PID = pcb_proceso -> PID;
		contexto_actualizado.PC = pcb_proceso -> PC;
		contexto_actualizado.registros = pcb_proceso -> registros;
		contexto_actualizado.TSegmento = pcb_proceso ->TSegmento;

		send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_actualizado);
		send_TABLA_SEGMENTOS(socket_cliente_kernel, contexto_actualizado.TSegmento);
		send_F_WRITE(socket_cliente_kernel,archivo,dir_fisica,cant_bytes);
		op_code cop;
		if (recv(socket_cliente_kernel, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
		//log_error(logger, "Error al recibir el code");
	}
	seguir_ejecutando=recv_seguir_ejecutando(socket_cliente_kernel);

		return seguir_ejecutando; //no corta la ejecucion de las instrucciones (se usa en execute_decode en recibo_instrucciones.c)
	}

}

int ejecutar_F_TRUNCATE(pcb_cpu* pcb_proceso, char* archivo, uint32_t tamanio){
	contexto_ejecucion contexto_actualizado;
	pcb_proceso -> PC += 1;

	contexto_actualizado.PID = pcb_proceso -> PID;
	contexto_actualizado.PC = pcb_proceso -> PC;
	contexto_actualizado.registros = pcb_proceso -> registros;
	contexto_actualizado.TSegmento = pcb_proceso ->TSegmento;

	send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_actualizado);
	send_TABLA_SEGMENTOS(socket_cliente_kernel, contexto_actualizado.TSegmento);
	send_F_TRUNCATE(socket_cliente_kernel,archivo,tamanio);
	op_code cop;
	if (recv(socket_cliente_kernel, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
		//log_error(logger, "Error al recibir el code");
	}
	seguir_ejecutando=recv_seguir_ejecutando(socket_cliente_kernel);
	return seguir_ejecutando;
}

int ejecutar_WAIT(pcb_cpu* pcb_proceso , char* recurso){
	contexto_ejecucion contexto_actualizado;
	pcb_proceso -> PC += 1;

	contexto_actualizado.PID = pcb_proceso -> PID;
	contexto_actualizado.PC = pcb_proceso -> PC;
	contexto_actualizado.registros = pcb_proceso -> registros;
	contexto_actualizado.TSegmento = pcb_proceso ->TSegmento;

	send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_actualizado);
	send_TABLA_SEGMENTOS(socket_cliente_kernel, contexto_actualizado.TSegmento);
	send_WAIT(socket_cliente_kernel, recurso);
	op_code cop;
	if (recv(socket_cliente_kernel, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
		//log_error(logger, "Error al recibir el code");
	}
	seguir_ejecutando=recv_seguir_ejecutando(socket_cliente_kernel);
	return seguir_ejecutando;
}

int ejecutar_SIGNAL(pcb_cpu* pcb_proceso , char* recurso){
	contexto_ejecucion contexto_actualizado;
	pcb_proceso -> PC += 1;

	contexto_actualizado.PID = pcb_proceso -> PID;
	contexto_actualizado.PC = pcb_proceso -> PC;
	contexto_actualizado.registros = pcb_proceso -> registros;
	contexto_actualizado.TSegmento = pcb_proceso ->TSegmento;

	send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_actualizado);
	send_TABLA_SEGMENTOS(socket_cliente_kernel, contexto_actualizado.TSegmento);
	send_SIGNAL(socket_cliente_kernel, recurso);
	op_code cop;
	if (recv(socket_cliente_kernel, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
		//log_error(logger, "Error al recibir el code");
	}
	seguir_ejecutando=recv_seguir_ejecutando(socket_cliente_kernel);
	return seguir_ejecutando;
}

int ejecutar_CREATE_SEGMENT(pcb_cpu* pcb_proceso, uint32_t id, uint32_t tamanio){
	contexto_ejecucion contexto_actualizado;
	pcb_proceso -> PC += 1;

	contexto_actualizado.PID = pcb_proceso -> PID;
	contexto_actualizado.PC = pcb_proceso -> PC;
	contexto_actualizado.registros = pcb_proceso -> registros;
	contexto_actualizado.TSegmento = pcb_proceso ->TSegmento;

	send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_actualizado);
	send_TABLA_SEGMENTOS(socket_cliente_kernel, contexto_actualizado.TSegmento);
	send_CREATE_SEGMENT(socket_cliente_kernel, id, tamanio);
	op_code cop;
	if (recv(socket_cliente_kernel, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
		//log_error(logger, "Error al recibir el code");
	}
	seguir_ejecutando=recv_seguir_ejecutando(socket_cliente_kernel);
	return seguir_ejecutando;
}

int ejecutar_DELETE_SEGMENT(pcb_cpu* pcb_proceso, uint32_t id){
	contexto_ejecucion contexto_actualizado;

	pcb_proceso -> PC += 1;

	contexto_actualizado.PID = pcb_proceso -> PID;
	contexto_actualizado.PC = pcb_proceso -> PC;
	contexto_actualizado.registros = pcb_proceso -> registros;
	contexto_actualizado.TSegmento = pcb_proceso ->TSegmento;

	send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_actualizado);
	send_TABLA_SEGMENTOS(socket_cliente_kernel, contexto_actualizado.TSegmento);
	send_DELETE_SEGMENT(socket_cliente_kernel,id);
	op_code cop;
	if (recv(socket_cliente_kernel, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
		//log_error(logger, "Error al recibir el code");
	}
	seguir_ejecutando=recv_seguir_ejecutando(socket_cliente_kernel);
	return seguir_ejecutando;
}

int ejecutar_YIELD(pcb_cpu* pcb_proceso){
	contexto_ejecucion contexto_actualizado;
	pcb_proceso -> PC += 1;
	contexto_actualizado.PID = pcb_proceso -> PID;
	contexto_actualizado.PC = pcb_proceso -> PC;
	contexto_actualizado.registros = pcb_proceso -> registros;
	contexto_actualizado.TSegmento = pcb_proceso ->TSegmento;

	send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_actualizado);
	send_TABLA_SEGMENTOS(socket_cliente_kernel, contexto_actualizado.TSegmento);
	send_YIELD(socket_cliente_kernel);

	//char buffer[4096];
	//while (recv(socket_cliente_kernel, buffer, sizeof(buffer), 0) > 0) {
    // Continuar recibiendo y descartando los datos del socket
    //memset(buffer, 0, sizeof(buffer));  // Limpiar el buffer para la siguiente iteración
	//}// prueba oscura


	//op_code cop;
	//if (recv(socket_cliente_kernel, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
	//	log_error(logger, "Error al recibir el code");
	//}
	//seguir_ejecutando=recv_seguir_ejecutando(socket_cliente_kernel);
	return 1;
}

int ejecutar_EXIT(pcb_cpu* pcb_proceso){
	contexto_ejecucion contexto_actualizado;

	contexto_actualizado.PID = pcb_proceso -> PID;
	contexto_actualizado.PC = pcb_proceso -> PC;
	contexto_actualizado.registros = pcb_proceso -> registros;
	contexto_actualizado.TSegmento = pcb_proceso ->TSegmento;

	send_CONTEXTO_EJECUCION(socket_cliente_kernel, contexto_actualizado);
	send_TABLA_SEGMENTOS(socket_cliente_kernel, contexto_actualizado.TSegmento);
	send_EXIT(socket_cliente_kernel);
	//op_code cop;
	//if (recv(socket_cliente_kernel, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
		//log_error(logger, "Error al recibir el code");
	//}
	//seguir_ejecutando=recv_seguir_ejecutando(socket_cliente_kernel);
	return 1;
}
