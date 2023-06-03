#include "configuraciones.h"

t_log* logger;
t_config* config;

char* ip;
char* puerto_cpu;
char* ip_memoria;
char* puerto_memoria;
uint32_t* retardo_instruccion;
char* tam_max_segmento;
int conexion;

void levantar_config(){
	ip = "127.0.0.1";
	puerto_cpu = config_get_string_value(config, "PUERTO_ESCUCHA");
	ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	puerto_memoria = config_get_string_value(config,"PUERTO_MEMORIA");
	retardo_instruccion = config_get_int_value(config,"RETARDO_INSTRUCCION"); //cambie de char a int para usarlo directamente
	tam_max_segmento = config_get_string_value(config,"TAM_MAX_SEGMENTO");
}
