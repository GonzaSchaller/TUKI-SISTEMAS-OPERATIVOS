#include "conexiones_kernel.h"

int contadorProcesos = 0 ;

//recibimos instrucciones, creamos pcb y lo mandamos a new
void procesar_conexion_consola(void* void_args){ //lo que hago por cada consola conectada
	args_atender_cliente* args = (args_atender_cliente*) void_args;
	t_log* log_kernel = args->log;
	int socket_cliente = args->socket;
	char* server_name = args->server_name;
	free(args);

	pcb_t *pcb = malloc(sizeof(pcb_t));
	t_list* lista_instrucciones = list_create();

	op_code cop;
	while(socket_cliente != -1){
		if(recv(socket_cliente, &cop, sizeof(op_code), 0) != sizeof(op_code)){ //si no recibo una instruccion
			//aca irian las cosas que recibo que no son una instruccion
//			contadorProcesos++;
//			inicializarPCB(lista_instrucciones,pcb); //inicializamos el pcb que le vamos a mandar al cpu
//			agregarANew(pcb); //agregamos cada proceso a NEW
			log_info(log_kernel, "No recibí una instruccion");
		}
		//creo que va aca
		contadorProcesos++;
		inicializarPCB(contadorProcesos,lista_instrucciones, pcb); //inicializamos el pcb que le vamos a mandar al cpu
		agregarANew(pcb); //agregamos cada proceso a NEW

		//me fijo que instruccion es segun el codigo de operacion
		 switch (cop) {
		 	case SET:
		 	{
		 		uint32_t parametro1;
		 		char* parametro2;
		 		if (!recv_SET(socket_cliente,&parametro1,&parametro2)) {
		 		     log_error(log_kernel, "Fallo recibiendo SET");
		 		     break;
		 		            	}
		 		cargar_instruccion2(SET, "SET", parametro1, parametro2, 0, lista_instrucciones);
				break;
		 	}
		 	case MOV_OUT:
		 			 	{
		 		uint32_t parametro1;
		 		uint32_t parametro2;
		 		if (!recv_MOV_OUT(socket_cliente, &parametro1, &parametro2)) {
		 			log_error(log_kernel, "Fallo recibiendo MOV_OUT");
		 			break;		 			 		            	}
		 		cargar_instruccion1(MOV_OUT, "MOV_OUT", parametro1, parametro2 , 0 , lista_instrucciones);
		 		break;
		 			 	}
		 	case WAIT: //parametro: Recurso
		 			{
		 		char* parametro1;
		 		if (!recv_WAIT(socket_cliente, &parametro1)) {
		 			log_error(log_kernel, "Fallo recibiendo WAIT");
		 			break;	            	}
		 			cargar_instruccion3(WAIT, "WAIT", parametro1 , 0 , 0 ,lista_instrucciones);
		 			break;
		 			 	}
            case IO:
            {
            	uint32_t parametro1;

            	if (!recv_IO(socket_cliente, &parametro1)) {
            	     log_error(log_kernel, "Fallo recibiendo IO");
            	     break;
            	}
            	cargar_instruccion1(IO,"IO",parametro1 , 0 , 0 ,lista_instrucciones);
            	break;
            }
            case SIGNAL: //parametro recurso
            {
            	char* parametro1;
            	if (!recv_SIGNAL(socket_cliente, &parametro1)) {
            		log_error(log_kernel, "Fallo recibiendo SIGNAL");
            		break;
            		 			}
            	cargar_instruccion3(SIGNAL, "SIGNAL", parametro1, 0 , 0 , lista_instrucciones);
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
            	cargar_instruccion1(MOV_IN, "MOV_IN", parametro1 , parametro2 , 0, lista_instrucciones);
            		break;
            		 	}
            case F_OPEN:
             {
            		char* parametro1;
            		if (!recv_F_OPEN(socket_cliente, &parametro1)) {
            		  log_error(log_kernel, "Fallo recibiendo F_OPEN");
            		  break;
            	  }
            		cargar_instruccion3(F_OPEN, "F_OPEN", parametro1, 0 , 0 , lista_instrucciones);
            		break;
            	 }
             case YIELD: //fijarse si poner rcv
             {
                  cargar_instruccion1(YIELD, "YIELD", 0 , 0 , 0 , lista_instrucciones);
                  break;
                   		 	}

			// ACA VOY YO
			  case F_TRUNCATE:
            {
            	char* parametro1;
				uint32_t parametro2;
            	if (!recv_F_TRUNCATE(socket_cliente, &parametro1, &parametro2)) {
            	   log_error(log_kernel, "Fallo recibiendo F_TRUNCATE");
            	   break;
            	}
            	cargar_instruccion3( F_TRUNCATE,"F_TRUNCATE", parametro1 , parametro2 , 0 ,lista_instrucciones);
            	break;
			}

			case F_SEEK:
            {
            	char* parametro1;
            	uint32_t parametro2;
            	if (!recv_F_SEEK(socket_cliente, &parametro1, &parametro2)) {
            	   log_error(log_kernel, "Fallo recibiendo F_SEEK");
            	   break;
            	}
            	cargar_instruccion3( F_SEEK,"F_SEEK", parametro1 , parametro2 , 0 ,lista_instrucciones);
            	break;
			}
			case CREATE_SEGMENT:
            {
            	uint32_t parametro1, parametro2;
            	if (!recv_CREATE_SEGMENT(socket_cliente, &parametro1, &parametro2)) {
            	   log_error(log_kernel, "Fallo recibiendo CREATE_SEGMENT");
            	   break;
            	}
            	cargar_instruccion1( CREATE_SEGMENT,"CREATE_SEGMENT", parametro1 , parametro2 , 0 ,lista_instrucciones);
            	break;
			}
			case F_WRITE:
            {
            	char* parametro1;
            	uint32_t parametro2, parametro3;
            	if (!recv_F_WRITE(socket_cliente, &parametro1, &parametro2, &parametro3)) {
            	   log_error(log_kernel, "Fallo recibiendo F_WRITE");
            	   break;
            	}
            	cargar_instruccion3( F_WRITE,"F_WRITE", parametro1 , parametro2 , parametro3 ,lista_instrucciones);
            	break;
			}
			case F_READ:
            {
            	char* parametro1;
            	uint32_t parametro2, parametro3;
            	if (!recv_F_READ(socket_cliente, &parametro1, &parametro2, &parametro3)) {
            	   log_error(log_kernel, "Fallo recibiendo F_READ");
            	   break;
            	}
            	cargar_instruccion3( F_READ,"F_READ", parametro1 , parametro2 , parametro3 ,lista_instrucciones);

            	break;
			}
			case DELETE_SEGMENT:
            {
            	uint32_t parametro1;

            	if (!recv_DELETE_SEGMENT(socket_cliente, &parametro1)) {
            	     log_error(log_kernel, "Fallo recibiendo DELETE_SEGMENT");
            	     break;
            	}
            	cargar_instruccion1(DELETE_SEGMENT,"DELETE_SEGMENT", parametro1 , 0 , 0 ,lista_instrucciones);

            	break;
            }
			case F_CLOSE:
            {
            	char* parametro1;

            	if (!recv_F_CLOSE(socket_cliente, &parametro1)) {
            	     log_error(log_kernel, "Fallo recibiendo F_CLOSE");
            	     break;
            	}
            	cargar_instruccion3(F_CLOSE,"F_CLOSE", parametro1 , 0 , 0 ,lista_instrucciones);

            	break;
            }
		    case EXIT:
            {
            	cargar_instruccion1(EXIT,"EXIT", 0 , 0 , 0 ,lista_instrucciones);
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
void escuchar_clientes(t_log* logger, int socket_servidor){

	args_atender_cliente* args = malloc(sizeof(args_atender_cliente));
	args->log = logger;
	args->socket = socket_servidor;
	args->server_name = "Kernel";
	while (1) {
	   pthread_t thread;
	   int *socket_cliente = malloc(sizeof(int));
	   *socket_cliente = esperar_cliente(logger, socket_servidor); //el server acepta al cliente y devuelve un socket_cliente
	   if(*socket_cliente != -1){ //si se establece la conexion
		   pthread_create(&thread, NULL, (void*) procesar_conexion_consola, (void*) args);
		   pthread_detach(thread);
	   }
	}
}

void agregarANew(pcb_t* pcb_proceso) {

	pthread_mutex_lock(&mutexNew);

	queue_push(colaNew, pcb_proceso);
	log_info(log_kernel, "[NEW] Entra el proceso de PID: %d a la cola.", pcb_proceso->PID);

	pthread_mutex_unlock(&mutexNew);

	sem_post(&contadorNew); // Despierta al planificador de largo plazo
	sem_post(&largoPlazo); // Ver si hay que usarlo

	//log_error(log_kernel,"Salgo de agregar a NEW tranqu
}
pcb_t* sacarDeNew(){

	sem_wait(&contadorNew); //si esta en 0 es que no hay mas pcbs para sacar, si estan en >0 es que hay para sacar
	pthread_mutex_lock(&mutexNew);

	pcb_t * pcb_proceso = queue_pop(colaNew);
	log_info(log_kernel, "[NEW] Se saca el proceso de PID: %d de la cola", pcb_proceso->PID);

	pthread_mutex_unlock(&mutexNew);

	return pcb_proceso;
}


void procesar_conexion_cpu(void* socket_cliente){


	free(socket_cliente);
}
// TODO
void procesar_conexion_fileSystem(void* socket_cliente){

	free(socket_cliente);
}
// TODO
void procesar_conexion_memoria(void* socket_cliente){

	free(socket_cliente);
}

