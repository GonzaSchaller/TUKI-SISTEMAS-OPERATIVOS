#include "file_system.h"

t_log* logger;
int fd_fs;
int fd_memoria;


int main (){

	// Issue 2967 es donde está la info de por qué definí las variables del .h en el .c también
	logger = log_create("fileSystem.log","FileSystem",1, LOG_LEVEL_DEBUG);

	levantar_config();

	fd_fs = iniciar_servidor(logger, "fileSystem", "127.0.0.1", c->puerto_escucha);
	fd_memoria = generar_conexion_con_memoria();
	conexion_kernel(logger,"filesystem",fd_fs);
	

	cargar_superbloque();
	cargar_bitmap();
	cargar_bloque();


	//terminar_fs();

	return EXIT_SUCCESS;

}
