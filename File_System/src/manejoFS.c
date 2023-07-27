#include "manejoFS.h"
extern t_config_fs *c;
extern t_log* logger;
extern t_superbloque* superbloque;
extern FILE* f_bloques;
t_list* lista_indirecta;

t_list* cant_bloques_puntero_indirecto(int puntero_indirecto, int cuantos_bloques_venia_usando){
	t_list*list_nro_de_bloques = list_create();
	//tengo que parsear el archivo de bloques para agarrar los bloques.
	fseek(f_bloques, superbloque->block_size*puntero_indirecto, SEEK_SET); // me paro en el bloquie indirecto para empezar a parsear
	 //aniadi el punero directo a la lista, es el primer bloque

	//list_add(list_nro_de_bloques,puntero_indirecto); // aniado al punero indirecto a la lista
	for(int i=0;i<cuantos_bloques_venia_usando;i++){ // le resto el directo y el indirecto y agrego los bloques que apunta el puntero indirecto osea la lista quedaria BLOQUE DIRECTO (puntero directo) + BLOQUE INDIRECTO (puntero al bloque indirecto) + BLOQUES DIRECTOS(punteros a los bloques directos que apunta el puntero directo)
		uint32_t* contenido_leido = malloc(sizeof(uint32_t));
		fread(contenido_leido, sizeof(uint32_t),1, f_bloques);
		list_add(list_nro_de_bloques,contenido_leido);
	}
 //SALE DE ACA CON UNA LISTA DE TODOS LOS BLOQUES QUE TIENE
 return list_nro_de_bloques;
}
int reverse_compare(void* element1, void* element2) {
	    // Comparing the elements in reverse order
	    int value1 = *(int*)element1;
	    int value2 = *(int*)element2;

	    if (value1 < value2) {
	        return 1;
	    } else if (value1 > value2) {
	        return -1;
	    } else { return 0;}
}

char* concat(char*nombre_archivo){
	//char*path = strdup(c->fcb);
	char* path = malloc(sizeof(char) * (strlen("fcb/") + strlen(nombre_archivo) + 1));
	strcpy(path, "fcb/");
	string_append(&path, nombre_archivo);
	//string_append(&path,nombre_archivo);

	return path;
}

bool existe_y_abrir(char*nombre_archivo){
	//*** verifica si existe el archivo ***
	char*path = concat(nombre_archivo);

	t_config* archivo = config_create(path);

	if(archivo == NULL) {
		log_info(logger,"El archivo <%s> NO existe", nombre_archivo);
		return false;
	}

	char*nombre = config_get_string_value(archivo,"NOMBRE_ARCHIVO");

	//*** abre el archivo ***
	//list_add(configs_creadas,(void*)archivo);
	if(strcmp(nombre,nombre_archivo)!=0) {
		log_error(logger,"Abri un archivo pero no coincide con su id, es esto posible? ");
		return false;
	}
	log_info(logger,"El archivo <%s> se abrio correctamente", nombre_archivo);
	return true;
}

bool crear_archivo(char*nombre,uint32_t tamanio){
	log_debug(logger,"Se empieza a crear el archivo");
	t_config* config = malloc(sizeof(t_config));
	config->path = concat(nombre);
	config->properties = dictionary_create();
	log_debug(logger,"Se termino de crear el config");

	 char tamanio_str[20];
	    sprintf(tamanio_str, "%d", tamanio);

	config_set_value(config,"NOMBRE_ARCHIVO",nombre);
	config_set_value(config,"TAMANIO_ARCHIVO",tamanio_str);
	config_set_value(config,"PUNTERO_DIRECTO",""); //la verdad que no dice nada de esto asi q -1 para no asociarle bloques.
	config_set_value(config,"PUNTERO_INDIRECTO","");
	log_debug(logger,"Termino de setear el config, (nombre, tamanio y punteros)");

	int guardar = config_save(config);
	if (guardar != -1)
		log_info(logger,"Archivo creado y seteado exitosamente");
	else {
		//log_error(logger,"Error al crear/setear el archivo");
		return false;
	}

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

t_list* bloque_del_archivo (fcb_t* fcb,uint32_t bloque_estoy,uint32_t cant_bloques_a_leer,uint32_t puntero,t_list** bloques_fs){
	//uint32_t bloque;
	t_list* bloques  = list_create();

	//uint32_t cant_bloques_archivo = ceil_casero(fcb->tamanio_archivo , superbloque->block_size);

	for(int i=0;i<cant_bloques_a_leer;i++){
		if(bloque_estoy <= 1){
			list_add(bloques,(void*)1);
			list_add(bloques_fs,(void*)fcb->puntero_directo);
			}
		else{
			//entro en el bloque indirecto
			uint32_t nro_bloque_leido;
			fseek(f_bloques,fcb->puntero_indirecto * superbloque->block_size+sizeof(uint32_t)*i, SEEK_SET);
			size_t bloque_leido = fread(&nro_bloque_leido,sizeof(uint32_t),1,bloques); //leo los bloques
			list_add(bloques,(void*)bloque_leido);
			list_add(bloques_fs,nro_bloque_leido);

		}
	}
	return bloques;
}
void leer_por_bloque(fcb_t* fcb, uint32_t puntero, uint32_t cant_bytes, char** contenido) {
	 	uint32_t tam_bloque = superbloque->block_size;
	    uint32_t enquebloqueestoy = ceil_casero(puntero, tam_bloque);
	    fseek(f_bloques, fcb->puntero_indirecto * tam_bloque, SEEK_SET);
	    fseek(f_bloques, (enquebloqueestoy - 1) * sizeof(uint32_t), SEEK_CUR);
	    uint32_t bloque_fs;
	    fread(&bloque_fs, sizeof(uint32_t), 1, f_bloques);
	    fseek(f_bloques, bloque_fs * tam_bloque, SEEK_SET);
	    uint32_t tamanio_restante = tam_bloque - puntero;
	    fseek(f_bloques, puntero%tam_bloque, SEEK_CUR);
	    char* contenido_leido;
	    if (tamanio_restante >= cant_bytes) { // Puedo leer sin salirme del bloque
	        fread(&contenido_leido, cant_bytes, 1,f_bloques);
	        strcat(*contenido, contenido_leido);
	    } else { // Me paso del bloque
	        uint32_t cant_a_leer_en_bloque_total = cant_bytes + tamanio_restante;
	        while (cant_a_leer_en_bloque_total > 0) {
	            uint32_t cuantos_bloques_venia_usando = ceil_casero(fcb->tamanio_archivo, superbloque->block_size);
	            t_list* lista_bloque_indirectos = cant_bloques_puntero_indirecto(fcb->puntero_indirecto, cuantos_bloques_venia_usando - 1);
	            uint32_t cant_bloques = list_size(lista_bloque_indirectos);

	                int posicion_actual = -1; // Inicializar la posición actual a un valor inválido

	                for (int i = 0; i < cant_bloques; i++) {
	                    uint32_t unbloque = *(uint32_t*)list_get(lista_bloque_indirectos, i);
	                    if (unbloque == enquebloqueestoy) {
	                        posicion_actual = i;
	                        break; // Salimos del bucle una vez que encontramos la posición actual
	                    }
	                }

	                if (posicion_actual != -1) {
	                    uint32_t bloque_siguiente = *(uint32_t*)list_get(lista_bloque_indirectos, posicion_actual + 1); // Agarro el siguiente bloque
	                    fseek(f_bloques, bloque_siguiente * tam_bloque, SEEK_SET);
	        	        fread(&contenido_leido, cant_bytes, 1,f_bloques);
	        	        strcat(*contenido, contenido_leido);
	                    cant_a_leer_en_bloque_total -= tam_bloque;
	                }
	        }
	    }

}
char* buscar_contenido(char*name,uint32_t puntero,uint32_t cant_bytes){
		uint32_t tam_bloque = superbloque->block_size;
		uint32_t cant_bloques; //cant de bloques a leer

		cant_bloques = ceil_casero(cant_bytes,tam_bloque);
		char*path = concat(name);
		t_config* archivo = config_create(path);

		if(archivo == NULL){
			log_error(logger, "Se esta intentando leer un archivo que NO existe, no se abrio previamente");
			return NULL;
		}

		fcb_t * fcb = malloc(sizeof(fcb_t));
		fcb->nombreArchivo = config_get_string_value(archivo,"NOMBRE_ARCHIVO");
		fcb->tamanio_archivo = config_get_int_value(archivo,"TAMANIO_ARCHIVO");
		fcb->puntero_directo= config_get_int_value(archivo,"PUNTERO_DIRECTO");
		fcb->puntero_indirecto = config_get_int_value(archivo,"PUNTERO_INDIRECTO");

		uint32_t enquebloqueestoy = ceil_casero(puntero,tam_bloque); //donde arranca // ese seria el bloque dentro del fs
		if(puntero%tam_bloque == 0){
			enquebloqueestoy++;
		}
		char* contenido_leido;
		if(enquebloqueestoy<=1){ //estoy en el directo
			fseek(f_bloques,fcb->puntero_directo * tam_bloque, SEEK_SET);
			fseek(f_bloques, puntero%tam_bloque, SEEK_CUR);
			uint32_t tamanio_restante = tam_bloque - puntero;
			if(tamanio_restante >= cant_bytes){
				fread(&contenido_leido,cant_bytes,1,f_bloques);
			}
			else{
				uint32_t cantidad_lectura_de_bloque = cant_bytes - tamanio_restante;
				fread(&contenido_leido,cantidad_lectura_de_bloque,1,f_bloques);
				leer_por_bloque(fcb, puntero, cant_bytes, &contenido_leido);
				//y me tengo que mvoer al sig bloque
			}
		}
		else if(enquebloqueestoy>1){
			leer_por_bloque(fcb,  puntero,  cant_bytes, &contenido_leido);
		}

		free(fcb);

		t_list* lista_bloques_fs;
		t_list* lista_de_bloques = bloque_del_archivo (fcb,enquebloqueestoy,cant_bloques,puntero,lista_bloques_fs);
		uint32_t tamanio = list_size(lista_de_bloques);

					for(int i=0;i<tamanio;i++){
						uint32_t nro_bloque = list_get(lista_de_bloques,i);
						uint32_t nro_bloque_fs = list_get(lista_bloques_fs,i);
						log_info(logger,"Acceso a Bloque: Acceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%d>",name,nro_bloque,nro_bloque_fs);
					}
				//supongo que es unu for por cada bloque.
	return contenido_leido;
}

void escribir_en_bloque(fcb_t* fcb, uint32_t puntero, uint32_t cant_bytes, char* contenido) {
    uint32_t tam_bloque = superbloque->block_size;
    uint32_t enquebloqueestoy = ceil_casero(puntero, tam_bloque);

    fseek(f_bloques, fcb->puntero_indirecto * tam_bloque, SEEK_SET);
    fseek(f_bloques, (enquebloqueestoy - 1) * sizeof(uint32_t), SEEK_CUR);
    uint32_t bloque_fs;
    fread(&bloque_fs, sizeof(uint32_t), 1, f_bloques);
    fseek(f_bloques, bloque_fs * tam_bloque, SEEK_SET);
    uint32_t tamanio_restante = tam_bloque - puntero;
    fseek(f_bloques, puntero%tam_bloque, SEEK_CUR);

    if (tamanio_restante >= cant_bytes) { // Puedo escribir sin salirme del bloque
        fwrite(contenido, cant_bytes, 1,f_bloques);
    } else { // Me paso del bloque
        uint32_t cant_a_escribir_en_bloque_total = cant_bytes + tamanio_restante;

        while (cant_a_escribir_en_bloque_total > 0) {
            uint32_t cuantos_bloques_venia_usando = ceil_casero(fcb->tamanio_archivo, superbloque->block_size);
            t_list* lista_bloque_indirectos = cant_bloques_puntero_indirecto(fcb->puntero_indirecto, cuantos_bloques_venia_usando - 1);
            uint32_t cant_bloques = list_size(lista_bloque_indirectos);

                int posicion_actual = -1; // Inicializar la posición actual a un valor inválido

                for (int i = 0; i < cant_bloques; i++) {
                    uint32_t unbloque = *(uint32_t*)list_get(lista_bloque_indirectos, i);
                    if (unbloque == enquebloqueestoy) {
                        posicion_actual = i;
                        break; // Salimos del bucle una vez que encontramos la posición actual
                    }
                }
                if (posicion_actual != -1) {
                    uint32_t bloque_siguiente = *(uint32_t*)list_get(lista_bloque_indirectos, posicion_actual + 1); // Agarro el siguiente bloque
                    fseek(f_bloques, bloque_siguiente * tam_bloque, SEEK_SET);
                    fwrite(contenido, tam_bloque, 1, f_bloques);
                    cant_a_escribir_en_bloque_total -= tam_bloque;
                }
        }
    }
}

// contenido de me pasaron de memoria lo tengo que escribir en archivo
bool escribir_contenido(char*name,char* contenido,uint32_t puntero,uint32_t cant_bytes){

		char*path = concat(name);
		t_config* archivo = config_create(path);

		if(archivo == NULL){
			log_error(logger, "Se esta intentando escribir en un archivo que NO existe, no se abrio previamente");
			return false;
		}

		fcb_t * fcb = malloc(sizeof(fcb_t));
		fcb->nombreArchivo = strdup(config_get_string_value(archivo,"NOMBRE_ARCHIVO"));
		fcb->tamanio_archivo = config_get_int_value(archivo,"TAMANIO_ARCHIVO");
		fcb->puntero_directo= config_get_int_value(archivo,"PUNTERO_DIRECTO");
		fcb->puntero_indirecto = config_get_int_value(archivo,"PUNTERO_INDIRECTO");

		uint32_t tam_bloque = superbloque->block_size;
		uint32_t cant_bloques; //cant de bloques a leer
		cant_bloques = ceil_casero(cant_bytes,tam_bloque);
		uint32_t enquebloqueestoy = ceil_casero(puntero,tam_bloque);
		if(puntero%tam_bloque == 0){
			enquebloqueestoy++;
		}

		if(enquebloqueestoy <= 1){
			fseek(f_bloques,fcb->puntero_directo * tam_bloque, SEEK_SET);

			uint32_t tamanio_restante = tam_bloque - puntero;
			if(tamanio_restante >= cant_bytes){ //puedo escribir sin salirme del bloque
				fseek(f_bloques, puntero%tam_bloque, SEEK_CUR);
				fwrite(contenido, 1,cant_bytes, f_bloques);
			}
			else{
				uint32_t cantidad_escritura_de_bloque = cant_bytes - tamanio_restante;
				fread(contenido,cantidad_escritura_de_bloque,1,f_bloques);
				escribir_en_bloque(fcb, puntero, cant_bytes, contenido);
			}
		}
		else if(enquebloqueestoy > 1) //estoy en el indirecto
				escribir_en_bloque(fcb, puntero, cant_bytes, contenido);

		//t_list* lista_bloques_fs = list_create();
		//t_list* lista_de_bloques = bloque_del_archivo (fcb,enquebloqueestoy,cant_bloques,puntero, &lista_bloques_fs);
//		uint32_t tamanio = list_size(lista_de_bloques);
//
//		for(int i=0;i<tamanio;i++){
//		uint32_t* nro_bloque = list_get(lista_de_bloques,i);
//	    uint32_t* nro_bloque_fs = list_get(lista_bloques_fs,i);
//		log_info(logger,"Acceso a Bloque: Acceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%d>",name,*nro_bloque,*nro_bloque_fs);
//		}

		free(fcb);

		return true;
}
void llenar_archivo_nuevo(fcb_t* fcb, t_config* archivo){
	uint32_t bloque_directo = buscar_bloque_libre();
	uint32_t bloque_indirecto = buscar_bloque_libre();
	fcb -> puntero_directo = bloque_directo;
	fcb-> puntero_indirecto = bloque_indirecto;
	char puntero_str[20];
	sprintf(puntero_str, "%d", fcb->puntero_directo);
	config_set_value(archivo,"PUNTERO_DIRECTO",puntero_str);
	sprintf(puntero_str, "%d",fcb->puntero_indirecto);
	config_set_value(archivo,"PUNTERO_INDIRECTO",puntero_str);
}
//cmentadifjlidf

uint32_t buscar_bloque_libre(){
	uint32_t tamanio_bitmap = bitarray_get_max_bit(bitarray);
	for(int i=0;i<tamanio_bitmap;i++){
		if(!bitarray_test_bit(bitarray, i)){
			bitarray_set_bit(bitarray,i);
			log_info(logger,"Acceso a Bitmap - Bloque: %d - Estado 1",i);
			return i;
		}
	}
	log_error(logger,"No hay bloques libres");
	return 0;
}

t_list * buscar_bloques_libres(int cant_bloques_a_agregar){
	t_list*bloques = list_create();
	for(int i =0; i<cant_bloques_a_agregar; i++){
		int* bloque = malloc(sizeof(int)); // Asignar memoria dinámicamente
		*bloque = buscar_bloque_libre();
		list_add(bloques, bloque);
	}
		return bloques;
}









