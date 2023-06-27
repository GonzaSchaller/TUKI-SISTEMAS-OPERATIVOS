#include "iniciar.h"
t_config_fs *c;
extern t_log* logger;
t_superbloque* superbloque;
t_config* config;
extern int fd_fs;
extern int fd_memoria;




void levantar_config(){
	c = malloc(sizeof(t_config_fs)); //para la config

	config = config_create("fileSystem.config");
	if(config ==NULL) log_error(logger,"no se encontro el config");

	c->ip_memoria  = config_get_string_value(config,"IP_MEMORIA");
	c->puerto_memoria = config_get_string_value(config,"PUERTO_MEMORIA");
	c->puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA");
	c->superbloque = config_get_string_value(config,"PATH_SUPERBLOQUE");
	c->bitmap = config_get_string_value(config,"PATH_BITMAP");
	c->bloques = config_get_string_value(config,"PATH_BLOQUES");
	c->fcb = config_get_string_value(config,"PATH_FCB");
	c->retardo_acceso_bloque = config_get_string_value(config,"RETARDO_ACCESO_BLOQUE");

	config_destroy(config);

}


void cargar_superbloque(){
	char* path = strdup(c->superbloque);
	t_config* cnf_fs = config_create(path);
	superbloque = malloc(sizeof(t_superbloque));
	if(cnf_fs == NULL) {
		        log_error(logger, "no se encontro el archivo del superbloque");
		    }
	superbloque->block_size = config_get_string_value(config,"BLOCK_SIZE");
	superbloque->block_count = config_get_string_value(config,"BLOCK_COUNT");
	free(cnf_fs);

}

void terminar_fs(){
	log_destroy(logger);

	close(fd_fs);
	close(fd_memoria);
}









