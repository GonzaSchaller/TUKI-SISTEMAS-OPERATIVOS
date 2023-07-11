#include "manejoFS.h"
extern t_config_fs *c;

char*PATH_STATIC=c->fcb;

char* concat(char*nombre_archivo){
	char*path = strdup(PATH_STATIC);
	string_append(&path,"/");
	string_append(&path,nombre_archivo);
	return path;
}

bool existe_y_abrir(char*nombre_archivo){
	//hay quer abrir el archivo
	char*path = concat(nombre_archivo);
	t_config* archivo = config_create(path);
	if(archivo==NULL) {
		log_error(logger,"el archivo NO existe");
		return false;
	}
	char*nombre = config_get_string_value(archivo,"NOMBRE");
	if(strcmp(nombre,nombre_archivo)!=0) {
		log_error(logger,"Abri un archivo pero no coincide con su id, es esto posible? ");
		return false;
	}
	return true;
}

bool crear_archivo(char*nombre,uint32_t tamanio){
	t_config* config = malloc(sizeof(t_config));
	config->path = concat(nombre);
	config->properties = dictionary_create();

	config_set_value(config,"NOMBRE_ARCHIVO",nombre);
	config_set_value(config,"TAMANIO_ARCHIVO",tamanio);
	config_set_value(config,"PUNTERO_DIRECTO","-1"); //la verdad que no dice nada de esto asi q -1 para no asociarle bloques.
	config_set_value(config,"PUNTERO_INDIRECTO","-1");

	int guardar = config_save(config);
	guardar != -1 ? log_info(logger,"archivo creado/seteado exitosamente") : log_error(logger,"algo malio sal");

	config_destroy(config);
	return true;
}

char* buscar_contenido(uint32_t puntero,uint32_t cb){




}





















