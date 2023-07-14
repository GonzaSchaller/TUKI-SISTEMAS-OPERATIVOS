#include "manejoFS.h"
extern t_config_fs *c;
extern t_log* logger;



char* concat(char*nombre_archivo){
	char*path = strdup(c->fcb);
	string_append(&path,"/");
	string_append(&path,nombre_archivo);
	return path;
}

bool existe_y_abrir(char*nombre_archivo){ //TODO DUDAS ver de partir esta funciones en varias para reutilizarlas
	//hay quer abrir el archivo
	char*path = concat(nombre_archivo); //TODO DUDAS (CONCAT): ver si nombre_archivo va así o con el * o &
	t_config* archivo = config_create(path);
	if(archivo == NULL) {
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
	char* nombre_archivo; //VER de donde saco el nombre
	char* b = NULL; //porque un char es un byte y cb es cantidad de bytes SINO CORREGIR
	uint32_t tam_bloque = config_get_int_value(c -> superbloque, "BLOCK_SIZE");
	FILE* f = fopen(nombre_archivo,"wb");
	if(f != NULL){
		fseek(f, sizeof(uint32_t)*tam_bloque, SEEK_SET);
		fread(b, sizeof(*b), cb, f);
		fclose(nombre_archivo);
	}
	return b; // si no entra en el if retorna NULL
}


// contenido de me pasaron de memoria lo tengo que escribir en archivo
bool escribir_contenido(char* contenido,uint32_t punterow){

	/* tema puntero:
		me pasan el puntero 12
		me muevo 12 * tamanio del bloque
		(dato: un char es 1 byte)
	*/

	// una onda así
	char* nombre_archivo; //TODO CONSEGUIR nombre_archivo
	uint32_t tam_bloque = config_get_int_value(c -> superbloque, "BLOCK_SIZE"); //REVISAR que accedi al superbloque bien
	FILE* f = fopen(nombre_archivo,"wb");
	if(f != NULL){
		fseek(f,sizeof(uint32_t)*tam_bloque,SEEK_SET);
		fwrite(contenido, sizeof(contenido), 1,f);
		fclose(nombre_archivo);
		return true;
	}
	return false;
}

















