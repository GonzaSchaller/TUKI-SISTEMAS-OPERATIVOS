#include "manejoFS.h"
extern t_config_fs *c;
extern t_log* logger;
extern t_superbloque* superbloque;
extern FILE* f_bloques;
//t_list* configs_creadas;

char* concat(char*nombre_archivo){
	//char*path = strdup(c->fcb);
	char* path = malloc(sizeof(char) * (strlen("fcb/") + strlen(nombre_archivo) + 1));
	strcpy(path, "fcb/");
	string_append(&path, nombre_archivo);
	//string_append(&path,nombre_archivo);

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

	char*nombre = config_get_string_value(archivo,"NOMBRE_ARCHIVO");



//	list_add(configs_creadas,(void*)archivo);
	if(strcmp(nombre,nombre_archivo)!=0) {
		log_error(logger,"Abri un archivo pero no coincide con su id, es esto posible? ");
		return false;
	}
	return true;
}



bool crear_archivo(char*nombre,uint32_t tamanio){
	log_info(logger,"entre");
	t_config* config = malloc(sizeof(t_config));
	config->path = concat(nombre);
	config->properties = dictionary_create();
	log_info(logger,"entre2");

	 char tamanio_str[20];
	    sprintf(tamanio_str, "%d", tamanio);

	config_set_value(config,"NOMBRE_ARCHIVO",nombre);
	config_set_value(config,"TAMANIO_ARCHIVO",tamanio_str);
	config_set_value(config,"PUNTERO_DIRECTO","-1"); //la verdad que no dice nada de esto asi q -1 para no asociarle bloques.
	config_set_value(config,"PUNTERO_INDIRECTO","-1");
	log_info(logger,"entre3");
	int guardar = config_save(config);
	guardar != -1 ? log_info(logger,"archivo creado/seteado exitosamente") : log_error(logger,"algo malio sal");

	//list_add(configs_creadas,(void*)config);
	//config_destroy(config);
	return true;
}

char* buscar_contenido(char*nombre,uint32_t puntero,uint32_t cant_bytes){


	//leo de a bloques, lo probe y anda.
	uint32_t tam_bloque = superbloque->block_size;
	uint32_t cant_bloques ;


	uint32_t cociente = cant_bytes / tam_bloque;
	uint32_t resto = cant_bytes% tam_bloque;


		    if (resto > 0) {
		        cant_bloques = cociente + 1;
		    } else {
		        cant_bloques = cociente;
		    }

		uint32_t ubicacion = puntero * tam_bloque;

		//supongo que es unu for por cada bloque.
		log_info(logger,"Acceso a Bloque: Acceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <>",nombre,puntero);

		fseek(f_bloques, sizeof(uint32_t) * ubicacion, SEEK_SET);
		char* contenido_leido = malloc(tam_bloque * cant_bloques);
		size_t elementos_leidos = fread(contenido_leido, tam_bloque,cant_bloques, f_bloques);

		log_info(logger, "contenido: %s,cantbloques%d", contenido_leido,elementos_leidos);

	return contenido_leido;
}


// contenido de me pasaron de memoria lo tengo que escribir en archivo
bool escribir_contenido(char* contenido,uint32_t punterow){

	/* tema puntero:
		me pasan el puntero 12
		me muevo 12 * tamanio del bloque
		(dato: un char es 1 byte)
	*/

	// una onda así
	//char*path = concat(archivo); //TODO DUDAS (CONCAT): ver si nombre_archivo va así o con el * o &
	//t_config* archivo = config_create(path);
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



fcb_t * buscar_fcb(nombre_archivo){
	//fcb_t fcb = malloc(sizeof(fcb_t));





}


/*
 * char*path = concat(name); //TODO DUDAS (CONCAT): ver si nombre_archivo va así o con el * o &
		t_config* archivo = config_create(path);

		fcb_t * fcb;
		fcb->nombreArchivo = config_get_string_value(config,"NOMBRE_ARCHIVO");
		fcb->tamanio_archivo = config_get_int_value(config,"TAMANIO_ARCHIVO");
		fcb->puntero_directo= config_get_int_value(config,"PUNTERO_DIRECTO");
		fcb->puntero_indirecto = config_get_int_value(config,"PUNTERO_INDIRECTO");
 *
 * */










