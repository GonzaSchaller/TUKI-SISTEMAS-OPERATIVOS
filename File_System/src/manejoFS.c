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
t_list* add_and_reverse(t_list* lista, int nuevo_elemento) {
    // Paso 1: Crear una nueva lista y agregar el nuevo elemento al final
    t_list* lista_temporal = list_create();
    list_add_all(lista_temporal, lista);
    list_add(lista_temporal, (void*)nuevo_elemento);

    // Paso 2: Revertir la nueva lista
    t_list* lista_invertida = list_create();
    for (int i = list_size(lista_temporal) - 1; i >= 0; i--) {
        int elemento = (int)list_get(lista_temporal, i);
        list_add(lista_invertida, (void*)elemento);
    }

    // Paso 3: Liberar la memoria de la lista temporal
    list_destroy(lista_temporal);

    // Paso 4: Devolver la lista invertida
    return lista_invertida;
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
		//log_info(logger,"El archivo <%s> NO existe", nombre_archivo);
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
	//log_debug(logger,"Se empieza a crear el archivo");
	t_config* config = malloc(sizeof(t_config));
	config->path = concat(nombre);
	config->properties = dictionary_create();
	//log_debug(logger,"Se termino de crear el config");

	 char tamanio_str[20];
	    sprintf(tamanio_str, "%d", tamanio);

	config_set_value(config,"NOMBRE_ARCHIVO",nombre);
	config_set_value(config,"TAMANIO_ARCHIVO",tamanio_str);
	config_set_value(config,"PUNTERO_DIRECTO",""); //la verdad que no dice nada de esto asi q -1 para no asociarle bloques.
	config_set_value(config,"PUNTERO_INDIRECTO","");
	//log_debug(logger,"Termino de setear el config, (nombre, tamanio y punteros)");

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


// Función para leer desde los bloques en el sistema de archivos
char* leer_en_bloque(fcb_t* fcb, uint32_t puntero, uint32_t cant_bytes, uint32_t enquebloquestoy) {

	uint32_t tam_bloque = superbloque->block_size;
	uint32_t cuantos_bloques_venia_usando = ceil_casero(fcb->tamanio_archivo, superbloque->block_size);
	t_list* lista_bloque_indirectos = cant_bloques_puntero_indirecto(fcb->puntero_indirecto, cuantos_bloques_venia_usando - 1);

	uint32_t bloque_fs = *(uint32_t*)list_get(lista_bloque_indirectos, enquebloquestoy-1);
	fseek(f_bloques, bloque_fs * tam_bloque, SEEK_SET);
	usleep(c->retardo_acceso_bloque*1000);
	log_info(logger,"Acceso a Bloque: “Acceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%d>",fcb->nombreArchivo,enquebloquestoy,bloque_fs);
	uint32_t tamanio_restante = tam_bloque - (puntero % tam_bloque);
    char* contenido_leido;
    char* contenido;
    if (tamanio_restante >= cant_bytes) {
        fread(&contenido, cant_bytes, 1, f_bloques);
        contenido_leido = strdup(contenido);
    } else {
        fread(contenido, tamanio_restante, 1, f_bloques);
        contenido_leido = strdup(contenido);
        int cant_a_leer_en_bloque_total = cant_bytes - tamanio_restante;
        uint32_t bytes_leidos = tamanio_restante;
        while (cant_a_leer_en_bloque_total > 0) {
        	int i = 0;

            uint32_t cuantos_bloques_venia_usando = ceil_casero(fcb->tamanio_archivo, superbloque->block_size);
            t_list* lista_bloque_indirectos = cant_bloques_puntero_indirecto(fcb->puntero_indirecto, cuantos_bloques_venia_usando - 1);
            uint32_t bloque_siguiente = *(uint32_t*)list_get(lista_bloque_indirectos, enquebloquestoy -1 + i);
            fseek(f_bloques, bloque_siguiente * tam_bloque, SEEK_SET);

            contenido = contenido + bytes_leidos;
            uint32_t bytes_a_leer_en_bloque = (cant_a_leer_en_bloque_total > tam_bloque) ? tam_bloque : cant_a_leer_en_bloque_total;
            fread(contenido, bytes_a_leer_en_bloque, 1, f_bloques);
            strcat(contenido_leido, contenido);

            log_info(logger,"Acceso a Bloque: “Acceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%d>",fcb->nombreArchivo,enquebloquestoy+1,bloque_siguiente);
            usleep(c->retardo_acceso_bloque*1000);
            bytes_leidos += bytes_a_leer_en_bloque;
            cant_a_leer_en_bloque_total -= bytes_a_leer_en_bloque;
            enquebloquestoy += i;
        }
    }
    return contenido_leido;
}

// Función para leer el contenido de un archivo en el sistema de archivos
char* buscar_contenido(char* name, uint32_t puntero, uint32_t cant_bytes) {
    char* path = concat(name);
    t_config* archivo = config_create(path);

    if (archivo == NULL) {
        //log_error(logger, "Se está intentando leer un archivo que NO existe, no se abrió previamente");
        return false;
    }

    fcb_t* fcb = malloc(sizeof(fcb_t));
    fcb->nombreArchivo = strdup(config_get_string_value(archivo, "NOMBRE_ARCHIVO"));
    fcb->tamanio_archivo = config_get_int_value(archivo, "TAMANIO_ARCHIVO");
    fcb->puntero_directo = config_get_int_value(archivo, "PUNTERO_DIRECTO");
    fcb->puntero_indirecto = config_get_int_value(archivo, "PUNTERO_INDIRECTO");

    uint32_t tam_bloque = superbloque->block_size;
    uint32_t enquebloqueestoy = -1 + ceil_casero((puntero + tam_bloque - 1), tam_bloque); // Bloque contando la posicion 0


    char* contenido_leido;
    char* contenido = malloc(cant_bytes);

    if (enquebloqueestoy == 0) {
        fseek(f_bloques, fcb->puntero_directo * tam_bloque, SEEK_SET);
        usleep(c->retardo_acceso_bloque*1000);
        log_info(logger,"Acceso a Bloque: “Acceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%d>",fcb->nombreArchivo,enquebloqueestoy,fcb->puntero_directo);
        uint32_t tamanio_restante = tam_bloque - (puntero % tam_bloque);
        if (tamanio_restante >= cant_bytes) {
            fseek(f_bloques, puntero % tam_bloque, SEEK_CUR);
            fread(contenido, cant_bytes, 1 , f_bloques);
            contenido_leido = strdup(contenido);
        } else {
            fread(contenido, tamanio_restante, 1, f_bloques);
            contenido_leido = strdup(contenido);
            contenido = leer_en_bloque(fcb, puntero, cant_bytes - tamanio_restante, enquebloqueestoy);
            strcat(contenido_leido,contenido);
        }
    }
    else if (enquebloqueestoy > 0) { // Estoy en el indirecto
    	contenido_leido = leer_en_bloque(fcb, puntero, cant_bytes, enquebloqueestoy);
    }

    free(fcb);
    return contenido_leido;
}

void escribir_en_bloque(fcb_t* fcb, uint32_t puntero, uint32_t cant_bytes, uint32_t enquebloquestoy, char* contenido) {
    uint32_t tam_bloque = superbloque->block_size;
    uint32_t cuantos_bloques_venia_usando = ceil_casero(fcb->tamanio_archivo, superbloque->block_size);
    t_list* lista_bloque_indirectos = cant_bloques_puntero_indirecto(fcb->puntero_indirecto, cuantos_bloques_venia_usando - 1);

    uint32_t bloque_fs = *(uint32_t*)list_get(lista_bloque_indirectos, enquebloquestoy-1);
    fseek(f_bloques, bloque_fs * tam_bloque, SEEK_SET);
    usleep(c->retardo_acceso_bloque*1000);

    uint32_t tamanio_restante = tam_bloque - (puntero % tam_bloque);
    log_info(logger,"Acceso a Bloque: “Acceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%d>",fcb->nombreArchivo,enquebloquestoy,bloque_fs);
    if (tamanio_restante >= cant_bytes) { // Puedo escribir sin salirme del bloque
        fwrite(contenido, cant_bytes, 1, f_bloques);

    } else { // Me paso del bloque
        fwrite(contenido, tamanio_restante, 1, f_bloques);

        int cant_a_escribir_en_bloque_total = cant_bytes - tamanio_restante;
        uint32_t bytes_escritos = tamanio_restante;
        while (cant_a_escribir_en_bloque_total > 0) {
        		int i = 0;
                uint32_t bloque_siguiente = *(uint32_t*)list_get(lista_bloque_indirectos, enquebloquestoy + i);

                fseek(f_bloques, bloque_siguiente * tam_bloque , SEEK_SET);
                // Calcular cuántos bytes se pueden escribir en este bloque
                uint32_t bytes_a_escribir_en_bloque = (cant_a_escribir_en_bloque_total > tamanio_restante) ? tamanio_restante : cant_a_escribir_en_bloque_total;
                // Escribir los bytes correspondientes en este bloque y actualizar la posición de escritura
                char* contenidoNuevo  = contenido + (strlen(contenido) - bytes_a_escribir_en_bloque);
                fwrite(contenidoNuevo, bytes_a_escribir_en_bloque, 1, f_bloques);
                usleep(c->retardo_acceso_bloque*1000);
                log_info(logger,"Acceso a Bloque: “Acceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%d>",fcb->nombreArchivo, enquebloquestoy + 1 ,bloque_siguiente);
                bytes_escritos += bytes_a_escribir_en_bloque;
                cant_a_escribir_en_bloque_total -= bytes_a_escribir_en_bloque;
                enquebloquestoy += i;
                i++;
        }
    }
}

bool escribir_contenido(char* name, char* contenido, uint32_t puntero, uint32_t cant_bytes) {
    char* path = concat(name);
    t_config* archivo = config_create(path);

    if (archivo == NULL) {
        log_error(logger, "Se está intentando escribir en un archivo que NO existe, no se abrió previamente");
        return false;
    }

    fcb_t* fcb = malloc(sizeof(fcb_t));
    fcb->nombreArchivo = strdup(config_get_string_value(archivo, "NOMBRE_ARCHIVO"));
    fcb->tamanio_archivo = config_get_int_value(archivo, "TAMANIO_ARCHIVO");
    fcb->puntero_directo = config_get_int_value(archivo, "PUNTERO_DIRECTO");
    fcb->puntero_indirecto = config_get_int_value(archivo, "PUNTERO_INDIRECTO");

    uint32_t tam_bloque = superbloque->block_size;

    uint32_t enquebloqueestoy = -1 + ceil_casero((puntero + tam_bloque - 1), tam_bloque); // Bloque contando la posicion 0

    if (enquebloqueestoy <= 0) {
        fseek(f_bloques, fcb->puntero_directo * tam_bloque, SEEK_SET);

        uint32_t tamanio_restante = tam_bloque - (puntero % tam_bloque);
        if (tamanio_restante >= cant_bytes) { // Puedo escribir sin salirme del bloque
            fseek(f_bloques, puntero % tam_bloque, SEEK_CUR);
            fwrite(contenido, cant_bytes, 1 , f_bloques);
            usleep(c->retardo_acceso_bloque*1000);
            log_info(logger,"Acceso a Bloque: “Acceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%d>",fcb->nombreArchivo,enquebloqueestoy,fcb->puntero_directo);
        } else {
            fwrite(contenido, tamanio_restante, 1, f_bloques);
            usleep(c->retardo_acceso_bloque*1000);
            log_info(logger,"Acceso a Bloque: “Acceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%d>",fcb->nombreArchivo,enquebloqueestoy,fcb->puntero_directo);
            escribir_en_bloque(fcb, puntero + tamanio_restante, cant_bytes - tamanio_restante, enquebloqueestoy, contenido + tamanio_restante);
        }
    }
    else if (enquebloqueestoy > 1) { // Estoy en el indirecto
        escribir_en_bloque(fcb, puntero, cant_bytes, enquebloqueestoy, contenido);
    }
    fseek(f_bloques, 0, SEEK_SET); //para actualizar el hexdump(?
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
	//log_error(logger,"No hay bloques libres");
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









