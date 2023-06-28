#include "iniciar.h"
t_config_fs *c;
extern t_log* logger;
t_superbloque* superbloque;
t_bitarray*bitarray;
t_config* config;
extern int fd_fs;
extern int fd_memoria;

void inicializar() {
    c = malloc(sizeof(t_config_fs));
    c->bitmap= NULL;
    c->bloques = NULL;
    c->fcb= NULL;
    c->ip_memoria= NULL;
    c->puerto_escucha = NULL;
    c->puerto_memoria = NULL;
    c->superbloque= NULL;

}

void levantar_config(){
	inicializar();

	config = config_create("fileSystem.config");
	if(config ==NULL) log_error(logger,"no se encontro el config");

	c->ip_memoria  = strdup(config_get_string_value(config,"IP_MEMORIA"));
	c->puerto_memoria = strdup(config_get_string_value(config,"PUERTO_MEMORIA"));
	c->puerto_escucha = strdup(config_get_string_value(config,"PUERTO_ESCUCHA"));
	c->superbloque = strdup(config_get_string_value(config,"PATH_SUPERBLOQUE"));
	c->bitmap = strdup(config_get_string_value(config,"PATH_BITMAP"));
	c->bloques = strdup(config_get_string_value(config,"PATH_BLOQUES"));
	c->fcb = strdup(config_get_string_value(config,"PATH_FCB"));
	c->retardo_acceso_bloque = (uint32_t) strtol(config_get_string_value(config, "RETARDO_ACCESO_BLOQUE"), NULL, 10);


	config_destroy(config);

}


void cargar_superbloque(){

	char* path = strdup(c->superbloque);
	log_info(logger,"%s",path);
	t_config* cnf_fs = config_create(path);

	superbloque = malloc(sizeof(t_superbloque));

	if(cnf_fs == NULL) {
		        log_error(logger, "no se encontro el archivo del superbloque");
		    }
	superbloque->block_size = config_get_string_value(config,"BLOCK_SIZE");
	superbloque->block_count = config_get_string_value(config,"BLOCK_COUNT");
	free(cnf_fs);

}


void cargar_bitmap(){

	char*path = strdup(c->bitmap);







}

void terminar_fs(){
	log_destroy(logger);
	close(fd_fs);
	close(fd_memoria);
}









