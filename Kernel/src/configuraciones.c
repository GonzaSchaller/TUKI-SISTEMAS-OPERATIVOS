#include "configuraciones.h"

char* ip;
char* puerto_escucha;
char* ip_cpu;
char* puerto_cpu;
char* ip_memoria;
char* puerto_memoria;
char* ip_fileSystem;
char* puerto_fileSystem;
char* algoritmo_planificacion;
uint32_t estimacion_inicial;
float hrrn_alfa;
uint32_t grado_max_multiprogramacion;
t_list* lista_recursos;


//si el hrrnAlfa es negativo cierro el programa
void validar_alfa(float alfa){
		if(alfa < 0){
			perror("No se pudo obtener el valor del archivo de configuraciones!\n");
			abort();
		}
}
//mete cada recurso en una lista con su instancia
t_list* obtener_recursos(t_config* config, char* rec, char* instancias){
	t_list* lista_recursos = list_create();
	char** array_recursos = config_get_array_value(config, rec);
	char** array_instancias = config_get_array_value(config, instancias);
	int posicion = 0;
	t_queue* cola_bloqueados = queue_create();

	while(array_recursos[posicion] != NULL){
		recurso_sistema* recurso = malloc(sizeof(recurso_sistema));
		recurso->nombre = array_recursos[posicion];
		recurso->instancia = atoi(array_instancias[posicion]); //capaz necesito chequear que no falle atoi
		recurso->colaBloqueados = cola_bloqueados;
		pthread_mutex_init(&recurso->mutexRecurso, NULL); //inicializo el mutex
		list_add(lista_recursos, recurso);
		posicion++;
		free(recurso);
	}

	return lista_recursos;
}
int obtener_algoritmo_planificacion(char* algoritmo){

	algoritmo_planif algoritmo_planif;


	    //HRRN
	    if (strcmp(algoritmo,"FIFO") == 0)
	    {
	    	algoritmo_planif = FIFO;
	        //log_info(log_kernel, "El algoritmo de planificacion elegido es FIFO.");
	    }

	    //SFJ SIN DESALOJO
	    if (strcmp(algoritmo,"HRRN") == 0)
	    {
	    	algoritmo_planif = HRRN;
	     //   log_info(log_kernel, "El algoritmo de planificacion elegido es SRT.");
	    }
	    return algoritmo_planif;
}



