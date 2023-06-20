#include "iniciar.h"
t_config_fs *c;
extern t_log* logger;


void levantar_config(){

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
}


void cargar_superbloque(){
	char* path = strdup(c->superbloque);
	FILE*f = fopen(path,"wb+");


}
