#include "planificador.h"

int contadorProcesos = 0 ;
//recibimos instrucciones, creamos pcb y lo mandamos a new
void atender_cliente(void* void_args){ //lo que hago por cada consola conectada
	args_atender_cliente* args = (args_atender_cliente*) void_args;
	t_log* log_kernel = args->log;
	int socket_cliente = args->socket;
	char* server_name = args->server_name;
	free(args);

	pcb_t *pcb = malloc(sizeof(pcb_t));
	t_list* lista_instrucciones = list_create();

	op_code cop;
	while(socket_cliente != -1){
		if(recv(socket_cliente, &cop, sizeof(op_code), 0) != sizeof(op_code)){
			//aca irian las cosas que recibo que no son una instruccion
//			contadorProcesos++;
//			inicializarPCB(lista_instrucciones,pcb); //inicializamos el pcb que le vamos a mandar al cpu
//			agregarANew(pcb); //agregamos cada proceso a NEW
			log_info(log_kernel, "No recibí una instruccion");
		}
		//creo que va aca
		contadorProcesos++;
		inicializarPCB(contadorProcesos,lista_instrucciones, pcb); //inicializamos el pcb que le vamos a mandar al cpu
		agregarANew(pcb, log_kernel); //agregamos cada proceso a NEW

		//me fijo que instruccion es segun el codigo de operacion
		 switch (cop) {
		 	case SET:
		 	{
		 		uint32_t parametro1;
		 		char* parametro2;
		 		if (!recv_SET(socket_cliente, parametro1,parametro2)) {
		 		     log_error(log_kernel, "Fallo recibiendo SET");
		 		     break;
		 		            	}
		 		cargar_instruccion(SET, "SET", parametro1, parametro2, 0, lista_instrucciones);
				break;
		 	}
		 	case MOV_OUT:
		 			 	{
		 		uint32_t parametro1;
		 		uint32_t parametro2;
		 		if (!recv_MOV_OUT(socket_cliente, &parametro1, &parametro2)) {
		 			log_error(log_kernel, "Fallo recibiendo MOV_OUT");
		 			break;		 			 		            	}
		 		cargar_instruccion(MOV_OUT, "MOV_OUT", parametro1, parametro2 , 0 , lista_instrucciones);
		 		break;
		 			 	}
		 	case WAIT:
		 			{
		 		uint32_t parametro1;
		 		if (!recv_WAIT(socket_cliente, &parametro1)) {
		 			log_error(log_kernel, "Fallo recibiendo WAIT");
		 			break;	            	}
		 			cargar_instruccion(WAIT, "WAIT", parametro1 , 0 , 0 ,lista_instrucciones);
		 			break;
		 			 	}
            case IO:
            {
            	uint32_t parametro1;

            	if (!recv_IO(socket_cliente, &parametro1)) {
            	     log_error(log_kernel, "Fallo recibiendo IO");
            	     break;
            	}
            	cargar_instruccion(IO,"IO",parametro1 , 0 , 0 ,lista_instrucciones);
            	break;
            }
            case SIGNAL:
            {
            	uint32_t parametro1;
            	if (!recv_SIGNAL(socket_cliente, &parametro1)) {
            		log_error(log_kernel, "Fallo recibiendo SIGNAL");
            		break;
            		 			}
            	cargar_instruccion(SIGNAL, "SIGNAL", parametro1, 0 , 0 , lista_instrucciones);
            		break;
            		 	}
            case MOV_IN:
            		 	{
            	uint32_t parametro1;
            	uint32_t parametro2;
            	if (!recv_MOV_IN(socket_cliente, &parametro1, &parametro2)) {
            		log_error(log_kernel, "Fallo recibiendo MOV_IN");
            		break;
            		 		            	}
            	cargar_instruccion(MOV_IN, "MOV_IN", parametro1 , parametro2 , 0, lista_instrucciones);
            		break;
            		 	}
            case F_OPEN:
             {
            		uint32_t parametro1;
            		if (!recv_F_OPEN(socket_cliente, &parametro1)) {
            		  log_error(log_kernel, "Fallo recibiendo F_OPEN");
            		  break;
            	  }
            		cargar_instruccion(F_OPEN, "F_OPEN", parametro1, 0 , 0 , lista_instrucciones);
            		break;
            	 }
             case YIELD:
             {
                  cargar_instruccion(YIELD, "YIELD", 0 , 0 , 0 , lista_instrucciones);
                  break;
                   		 	}

			// ACA VOY YO
			  case F_TRUNCATE:
            {
            	uint32_t parametro1, parametro2;
            	if (!recv_F_TRUNCATE(socket_cliente, &parametro1, &parametro2)) {
            	   log_error(log_kernel, "Fallo recibiendo F_TRUNCATE");
            	   break;
            	}
            	cargar_instruccion( F_TRUNCATE,"F_TRUNCATE", parametro1 , parametro2 , 0 ,lista_instrucciones);
            	break;
			}

			case F_SEEK:
            {
            	uint32_t parametro1, parametro2;
            	if (!recv_F_SEEK(socket_cliente, &parametro1, &parametro2)) {
            	   log_error(log_kernel, "Fallo recibiendo F_SEEK");
            	   break;
            	}
            	cargar_instruccion( F_SEEK,"F_SEEK", parametro1 , parametro2 , 0 ,lista_instrucciones);
            	break;
			}
			case CREATE_SEGMENT:
            {
            	uint32_t parametro1, parametro2;
            	if (!recv_CREATE_SEGMENT(socket_cliente, &parametro1, &parametro2)) {
            	   log_error(log_kernel, "Fallo recibiendo CREATE_SEGMENT");
            	   break;
            	}
            	cargar_instruccion( CREATE_SEGMENT,"CREATE_SEGMENT", parametro1 , parametro2 , 0 ,lista_instrucciones);
            	break;
			}
			case F_WRITE:
            {
            	uint32_t parametro1, parametro2, parametro3 ;
            	if (!recv_F_WRITE(socket_cliente, &parametro1, &parametro2, &parametro3)) {
            	   log_error(log_kernel, "Fallo recibiendo F_WRITE");
            	   break;
            	}
            	cargar_instruccion( F_WRITE,"F_WRITE", parametro1 , parametro2 , parametro3 ,lista_instrucciones);
            	break;
			}
			case F_READ:
            {
            	uint32_t parametro1, parametro2, parametro3 ;
            	if (!recv_F_READ(socket_cliente, &parametro1, &parametro2, &parametro3)) {
            	   log_error(log_kernel, "Fallo recibiendo F_READ");
            	   break;
            	}
            	cargar_instruccion( F_READ,"F_READ", parametro1 , parametro2 , parametro3 ,lista_instrucciones);

            	break;
			}
			case DELETE_SEGMENT:
            {
            	uint32_t parametro1;

            	if (!recv_DELETE_SEGMENT(socket_cliente, &parametro1)) {
            	     log_error(log_kernel, "Fallo recibiendo DELETE_SEGMENT");
            	     break;
            	}
            	cargar_instruccion(DELETE_SEGMENT,"DELETE_SEGMENT", parametro1 , 0 , 0 ,lista_instrucciones);

            	break;
            }
			case F_CLOSE:
            {
            	uint32_t parametro1;

            	if (!recv_F_CLOSE(socket_cliente, &parametro1)) {
            	     log_error(log_kernel, "Fallo recibiendo F_CLOSE");
            	     break;
            	}
            	cargar_instruccion(F_CLOSE,"F_CLOSE", parametro1 , 0 , 0 ,lista_instrucciones);

            	break;
            }
		    case EXIT:
            {
            	cargar_instruccion(EXIT,"EXIT", 0 , 0 , 0 ,lista_instrucciones);
				break;
			}

            default:
                log_error(log_kernel, "Algo anduvo mal en el server de %s", server_name);
                log_info(log_kernel, "Cop: %d", cop);
                return;
        }
    }
    log_warning(log_kernel, "El cliente se desconecto de %s server", server_name);


    return;
}
void agregarANew(pcb_t* pcb_proceso, t_log* log_kernel) {

	pthread_mutex_lock(&mutexNew);

	queue_push(colaNew, pcb_proceso);
	log_info(log_kernel, "[NEW] Entra el proceso de PID: %d a la cola.", pcb_proceso->PID);

	pthread_mutex_unlock(&mutexNew);


	sem_post(&contadorNew); // Despierta al planificador de largo plazo
	sem_post(&largoPlazo); // Verifica que se haya despertado el planificador de largo plazo

	//log_error(log_kernel,"Salgo de agregar a NEW tranqu
}
pcb_t* sacarDeNew(pcb_t* pcb_proceso,t_log*log_kernel){

	sem_wait(&contadorNew);
	pthread_mutex_lock(&mutexNew);

	pcb_t* proceso = queue_pop(colaNew);
	log_info(log_kernel, "[NEW] Se saca el proceso de PID: %d de la cola", pcb_proceso->PID);

	pthread_mutex_unlock(&mutexNew);

	return pcb_proceso;
}

void agregarAReady(pcb_t* pcb,t_log* log_kernel){ // Tendria mas sentido que log_kernel no sea pasado por parametro
	//log_trace(log_kernel,"Entre en agregar a ready");

	time_t a = time(NULL);

	pthread_mutex_lock(&mutexReady);


	list_add(listaReady, pcb);

	log_info(log_kernel, "[READY] Entra el proceso de PID: %d a la cola.", pcb->PID);
//	send_TAM(server_memoria,METER_EN_MEM_PRINCIPAL);
//	send_TAM(server_memoria,proceso->indice_tabla_paginas);

	uint32_t indice_proceso; //ESTE RECV ES PARA SABER SI MEMORIA YA TERMINO DE PASAR A SWAP AL PROCESO SUSPENDIDO
	if (recv(server_memoria, &indice_proceso, sizeof(uint32_t), 0) != sizeof(uint32_t)) {
	log_info(log_kernel, "fallo al recibir nro de pagina!");
	return;
	}if(indice_proceso == 5555){
		log_trace(log_kernel,"Memoria termino de meter al proceso");
	}


	//printf("PROCESOS EN READY: %d \n", list_size(colaReady));
	log_debug(log_kernel,"[----------------PROCESOS EN READY: %d --------------------]\n", list_size(listaReady));

	pthread_mutex_unlock(&mutexReady);
	sem_post(&contadorReady);
	//sem_post(&contadorProcesosEnMemoria); Lo sacamos de aca para usarlo en el contexto en el que se llama a la funcion, porque no siempre que se agrega a ready, se toca la multiprogramacion
}


