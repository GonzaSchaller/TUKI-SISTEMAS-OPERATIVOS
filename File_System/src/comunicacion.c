#include "comunicacion.h"
extern t_log* logger;

void procesar_peticiones(int cliente_socket){
	op_code cop;
	while(cliente_socket!=-1){
		if(recv(cliente_socket,&cop,sizeof(op_code),0) == 0){
			log_info(logger,"DISCONECT!");
				return;  }


				switch(cop){

				case F_OPEN:
				break;


				   case F_WRITE:

					   break;
				   case F_READ:
					   break;

				   case F_TRUNCATE:
					   break;


				}

}

