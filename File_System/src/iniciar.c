#include "iniciar.h"
t_config_fs *c;
extern t_log* logger;
t_superbloque* superbloque;
t_config* config;
FILE* f_bloques;
FILE* f_bitmap;
t_bitarray* bitarray;
int fd_memoria;
char* ip;
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
	ip = config_get_string_value(config, "IP");
	c->ip_memoria  = strdup(config_get_string_value(config,"IP_MEMORIA"));
	c->puerto_memoria = strdup(config_get_string_value(config,"PUERTO_MEMORIA"));
	c->puerto_escucha = strdup(config_get_string_value(config,"PUERTO_ESCUCHA"));
	c->superbloque = strdup(config_get_string_value(config,"PATH_SUPERBLOQUE"));
	c->bitmap = strdup(config_get_string_value(config,"PATH_BITMAP"));
	c->bloques = strdup(config_get_string_value(config,"PATH_BLOQUES"));
	c->fcb = strdup(config_get_string_value(config,"PATH_FCB"));
	c->retardo_acceso_bloque = config_get_int_value(config, "RETARDO_ACCESO_BLOQUE");
	//log_info(logger,"levante config ok");
	config_destroy(config);

}


void cargar_superbloque(){

	char* path = strdup(c->superbloque);

	t_config* cnf_fs = config_create(path);

	superbloque = malloc(sizeof(t_superbloque));

	if(cnf_fs == NULL) {
		        log_error(logger, "no se encontro el archivo del superbloque");

		    }
	superbloque->block_size = (uint32_t) strtol(config_get_string_value(cnf_fs, "BLOCK_SIZE"), NULL, 10);

	superbloque->block_count = (uint32_t) strtol(config_get_string_value(cnf_fs, "BLOCK_COUNT"), NULL, 10);

	log_info(logger,"Se levanto correctamente el superbloque");

	free(cnf_fs);

}


void cargar_bitmap(){

	char*path = strdup(c->bitmap);

	FILE* f_bitmap = fopen(path,"rb+");
	if(f_bitmap==NULL) log_error(logger,"error abriendo archivo de bitmap");
	int fd = fileno(f_bitmap);
	uint32_t size_bitmap = ceil(superbloque->block_count / 8);
	lseek(fd, superbloque->block_count - 1, SEEK_SET);
	write(fd, "", 1);
	char *bitmap_de_bloques = mmap(NULL,size_bitmap, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

	if (bitmap_de_bloques == MAP_FAILED) {
	        perror("Error al mapear el archivo");
	        fclose(f_bitmap);
	    }
	//bitarray = bitarray_create(bitmap_de_bloques,size_bitmap);
	//hay que usar la nueva funcion
	 bitarray = bitarray_create_with_mode(bitmap_de_bloques,size_bitmap, LSB_FIRST);

	uint32_t cant = bitarray_get_max_bit(bitarray);
	log_info(logger,"Bitmap.dat abierto. Bits: %d",cant);

	//fclose(f_bitmap); //todo ver cuando cerrar
	free(path);
}

void cargar_bloque(){
	char*path = strdup(c->bloques);
	f_bloques = fopen(path,"rb+");
	if(f_bloques!=NULL){
	log_info(logger,"Bloques.dat abierto");}
	else log_error(logger,"error");
	//uint32_t* bloque = 3;
	//fwrite(bloque, sizeof(uint32_t),1, f_bloques);
}

void terminar_fs(){
	log_destroy(logger);
	//fclose(f_bloques);

	free(c);
	free(superbloque);
	free(bitarray);

}









