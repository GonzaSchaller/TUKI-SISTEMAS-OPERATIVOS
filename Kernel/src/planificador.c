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
		inicializarPCB(contadorProcesos,lista_instrucciones,pcb); //inicializamos el pcb que le vamos a mandar al cpu
		//agregarANew(pcb, log_kernel); //agregamos cada proceso a NEW
		//me fijo que instruccion es segun el codigo de operacion
		 switch (cop) {
		 	case SET:
		 	{
		 		uint32_t parametro1;
		 		uint32_t parametro2;
		 		if (!recv_SET(socket_cliente, &parametro1, &parametro2)) {
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
            	cargar_instruccion(IO,"I\O",parametro1 , 0 , 0 ,lista_instrucciones);
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
