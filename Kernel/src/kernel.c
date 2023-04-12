#include "kernel.h"


int main (){
		t_log* logger = log_create("kernel.log", "Kernel", 1, LOG_LEVEL_DEBUG);

		int server_fd = iniciar_servidor(logger,"127.0.0.1","50150");
		log_info(logger, "Servidor listo para recibir cliente");
		int cliente_fd = esperar_cliente(logger, server_fd);
	        //  Tenemos la conexion con la consola



	     	return EXIT_SUCCESS;


}
