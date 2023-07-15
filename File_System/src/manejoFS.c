#include "manejoFS.h"
extern t_config_fs *c;
extern t_log* logger;
extern t_superbloque* superbloque;
extern FILE* f_bloques;
t_list* lista_indirecta;

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


//cantidad de bytes,,, tamanio de bloque
uint32_t ceil_casero(uint32_t nro1,uint32_t nro2){

	uint32_t cociente = nro1 / nro2;
	uint32_t resto = nro1% nro2;

	return (resto > 0) ? (cociente + 1) : cociente;


}

t_list* bloque_del_archivo (fcb_t* fcb,uint32_t bloque_estoy,uint32_t cant_bloques_a_leer,uint32_t puntero){
	uint32_t bloque;
	t_list* bloques;
	uint32_t cant_bloques_archivo = ceil_casero(fcb->tamanio_archivo / superbloque->block_size);

	for(int i=0;i<cant_bloques_a_leer;i++){
		if(bloque_estoy == fcb->puntero_directo){
			list_add(bloques,0);
			cant_bloques_a_leer --;
			i++;
			}
		else{
			//entro en el bloque indirecto
			uint32_t nro_bloque_leido;
			fseek(f_bloques,fcb->puntero_indirecto * superbloque->block_size, SEEK_SET);
			size_t cant_leida = fread(&nro_bloque_leido,sizeof(uint32_t),1,bloques);
			if(nro_bloque_leido == bloque_estoy){
				list_add(bloques,cant_leida);
				if(cant_bloques_a_leer > 0){
					for(int u=0;u<cant_bloques_a_leer;u++){
						list_add(bloques,cant_leida+u);
					}
					return bloques;
				}
			}
		}
	}
	//en el de las config tendria 16 punteros en cada bloque de punteros.

	//4 bytes de puntero en un bloque cuantos punteros tengo?
	uint32_t cant_puteros_por_bloque = superbloque ->block_size / 4;

	return bloques;
}
char* buscar_contenido(char*name,uint32_t puntero,uint32_t cant_bytes){
		uint32_t tam_bloque = superbloque->block_size;
		uint32_t cant_bloques; //cant de bloques a leer

		cant_bloques = ceil_casero(cant_bytes,tam_bloque);
		    //la ubicacion esta en el puntero.

		uint32_t ubicacion = puntero;
		uint32_t enquebloqueestoy = ceil_casero(puntero,tam_bloque); //donde arranca // ese seria el bloque dentro de todo el fs

		char*path = concat(name); //TODO DUDAS (CONCAT): ver si nombre_archivo va así o con el * o &
		t_config* archivo = config_create(path);

		fcb_t * fcb;
		fcb->nombreArchivo = config_get_string_value(config,"NOMBRE_ARCHIVO");
		fcb->tamanio_archivo = config_get_int_value(config,"TAMANIO_ARCHIVO");
		fcb->puntero_directo= config_get_int_value(config,"PUNTERO_DIRECTO");
		fcb->puntero_indirecto = config_get_int_value(config,"PUNTERO_INDIRECTO");

		t_list* lista_de_bloques = bloque_del_archivo (fcb,enquebloqueestoy,cant_bloques,puntero);

		uint32_t tamanio = list_size(lista_de_bloques);

			for(int i=0;i<tamanio;i++){
				uint32_t nro_bloque = list_get(lista_de_bloques,i);
				log_info(logger,"Acceso a Bloque: Acceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <>",name,nro_bloque,enquebloqueestoy);
			}
		//supongo que es unu for por cada bloque.

		fseek(f_bloques, cant_bytes, SEEK_SET);
		char* contenido_leido = malloc(cant_bytes);
		size_t elementos_leidos = fread(contenido_leido, 1,cant_bytes, f_bloques);



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










