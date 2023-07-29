#include "file_system.h"

t_log* logger;
int fd_fs;


int main (){
	// Issue 2967 es donde está la info de por qué definí las variables del .h en el .c también
	logger = log_create("fileSystem.log","FileSystem",1, LOG_LEVEL_DEBUG);

	levantar_config();

	fd_fs = iniciar_servidor(logger, "fileSystem", "192.168.1.50", c->puerto_escucha);
	generar_conexion_con_memoria();
	
	cargar_superbloque();
	cargar_bitmap();
	cargar_bloque();

	conexion_kernel(logger,"filesystem",fd_fs);
	//terminar_fs();

	return EXIT_SUCCESS;

}
