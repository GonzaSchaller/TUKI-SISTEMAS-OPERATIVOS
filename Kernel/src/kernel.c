#include "kernel.h"

//lee el archivo config
void iniciar_config(t_config* config){
	puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
	ip_memoria = config_get_string_value(config,"IP_MEMORIA");
	puerto_memoria = config_get_string_value(config,"PUERTO_MEMORIA");
	//log_info(log_kernel, "Memoria: Lei la ip %s y el puerto %s\n", ip_memoria, puerto_memoria); // hacer con cada uno
	ip_cpu= config_get_string_value(config,"IP_CPU");
	puerto_cpu = config_get_string_value(config,"PUERTO_CPU");
	ip_fileSystem = config_get_string_value(config,"IP_FILESYSTEM");
	puerto_fileSystem = config_get_string_value(config,"PUERTO_FILESYSTEM");
	//ALGORITMO_PLANIFICACION=HRRN
	estimacion_inicial= config_get_int_value(config,"ESTIMACION_INICIAL" );
	//HRRN_ALFA=0.5
	grado_max_multiprogramacion = config_get_int_value(config,"GRADO_MAX_MULTIPROGRAMACION");
	//RECURSOS=[DISCO, RECURSO_1]
	//INSTANCIAS_RECURSOS=[1, 2]
}



void liberarConexiones(int socket1, int socket2, int socket3){
	if(socket1 != -1){
		close(socket1);
	}
	if(socket2 != -1){
		close(socket2);
	}
	if(socket3 != -1){
		close(socket3);
	}
}
void terminar_kernel(t_log* logger,t_config* config){
	if(logger !=NULL){
		log_destroy(logger);
	}
	if(config != NULL){
		config_destroy(config);
	}
}

typedef struct{
	int socket;
	t_log* log;
	char* server_name;
}args_atender_cliente;

//seguramente el socket_cliente tenga que estar dentor de una estructura
void atender_cliente(void* void_args){ //lo que hago por cada consola conectada
	args_atender_cliente* args = (args_atender_cliente*) void_args;
	t_log* log_kernel = args->log;
	int socket_cliente = args->socket;
	char* server_name = args->server_name;
	free(args);

	pcb_t *pcb = malloc(sizeof(pcb_t));
	t_list* lista_instrucciones = list_create();



	op_code cop;
	while(socket_cliente != -1){
		if(recv(socket_cliente, &cop, sizeof(op_code), 0) != sizeof(op_code)){
			contadorProcesos++;
			inicializarPCB(lista_instrucciones,pcb); //inicializamos el pcb que le vamos a mandar al cpu
			agregarANew(pcb); //agregamos cada proceso a NEW
		}
		//me fijo que instruccion es segun el codigo de operacion
		 switch (cop) {
		 	case SET:
		 	{
		 		uint32_t parametro1;
		 		uint32_t parametro2;
		 		if (!recv_SET(socket_cliente, &parametro1, &parametro2)) {
		 		     log_error(log_kernel, "Fallo recibiendo SET");
		 		     break;
		 		            	}
		 		cargar_intruccion2(SET, "SET", parametro1, parametro2, (uint32_t)NULL, lista_instrucciones);
				break;
		 	}
		 	case MOV_OUT:
		 			 	{
		 		uint32_t parametro1;
		 		uint32_t parametro2;
		 		if (!recv_MOV_OUT(socket_cliente, &parametro1, &parametro2)) {
		 			log_error(log_kernel, "Fallo recibiendo MOV_OUT");
		 			break;		 			 		            	}
		 		cargar_intruccion2(MOV_OUT, "MOV_OUT", parametro1, parametro2, (uint32_t)NULL, lista_instrucciones);
		 		break;
		 			 	}
		 	case WAIT:
		 			{
		 		uint32_t parametro1;
		 		if (!recv_WAIT(socket_cliente, &parametro1)) {
		 			log_error(log_kernel, "Fallo recibiendo WAIT");
		 			break;	            	}
		 			cargar_intruccion1(WAIT, "WAIT", parametro1, (uint32_t)NULL, lista_instrucciones);
		 			break;
		 			 	}
            case IO:
            {
            	uint32_t parametro1;

            	if (!recv_IO(socket_cliente, &parametro1)) {
            	     log_error(log_kernel, "Fallo recibiendo IO");
            	     break;
            	}
            	cargar_instruccion1(IO,"I\O",parametro1,(uint32_t)NULL,lista_instrucciones);
            	break;
            }
            case SIGNAL:
            {
            	uint32_t parametro1;
            	if (!recv_SIGNAL(socket_cliente, &parametro1)) {
            		log_error(log_kernel, "Fallo recibiendo SIGNAL");
            		break;
            		 			}
            	cargar_intruccion1(SIGNAL, "SIGNAL", parametro1, (uint32_t)NULL, lista_instrucciones);
            		break;
            		 	}
            case MOV_IN:
            		 	{
            	uint32_t parametro1;
            	uint32_t parametro2;
            	if (!recv_MOV_IN(socket_cliente, &parametro1, &parametro2)) {
            		log_error(log_kernel, "Fallo recibiendo MOV_IN");
            		break;
            		 		            	}
            	cargar_intruccion2(MOV_IN, "MOV_IN", parametro1, parametro2, (uint32_t)NULL, lista_instrucciones);
            		break;
            		 	}
            case F_OPEN:
             {
            		uint32_t parametro1;
            		if (!recv_F_OPEN(socket_cliente, &parametro1)) {
            		  log_error(log_kernel, "Fallo recibiendo F_OPEN");
            		  break;
            	  }
            		cargar_intruccion2(F_OPEN, "F_OPEN", parametro1,(uint32_t)NULL, lista_instrucciones);
            		break;
            	 }
             case YIELD:
             {
        if (!recv_YIELD(socket_cliente)) {
           log_error(log_kernel, "Fallo recibiendo YIELD");
                   	 break;
                  }
                  cargar_intruccion(YIELD, "YIELD",(uint32_t)NULL, lista_instrucciones);
                  break;
                   		 	}

			// ACA VOY YO
			  case F_TRUNCATE:
            {
            	uint32_t parametro1, parametro2;
            	if (!recv_F_TRUNCATE(socket_cliente, &parametro1, &parametro2)) {
            	   log_error(log_kernel, "Fallo recibiendo F_TRUNCATE");
            	   break;
            	}
            	cargar_instruccion2( F_TRUNCATE,"F_TRUNCATE",parametro1,parametro2,lista_instrucciones);
            	//log_warning(log_kernel, "Deserialice WRITE el parametro1 es: %d",parametro1);
            	//log_warning(log_kernel, "Deserialice WRITE el parametro2 es: %d",parametro2);
            	break;
			}

			case CREATE_SEGMENT:
            {
            	uint32_t parametro1, parametro2;
            	if (!recv_CREATE_SEGMENT(socket_cliente, &parametro1, &parametro2)) {
            	   log_error(log_kernel, "Fallo recibiendo CREATE_SEGMENT");
            	   break;
            	}
            	cargar_instruccion2( CREATE_SEGMENT,"CREATE_SEGMENT",parametro1,parametro2,lista_instrucciones);
            	break;
			}
			case F_SEEK:
            {
            	uint32_t parametro1, parametro2;
            	if (!recv_F_SEEK(socket_cliente, &parametro1, &parametro2)) {
            	   log_error(log_kernel, "Fallo recibiendo F_SEEK");
            	   break;
            	}
            	cargar_instruccion2( F_SEEK,"F_SEEK",parametro1,parametro2,lista_instrucciones);
            	break;
			}
			case F_WRITE:
            {
            	uint32_t parametro1, parametro2, parametro3 ;
            	if (!recv_F_WRITE(socket_cliente, &parametro1, &parametro2, &parametro3)) {
            	   log_error(log_kernel, "Fallo recibiendo F_WRITE");
            	   break;
            	}
            	cargar_instruccion3( F_WRITE,"F_WRITE",parametro1,parametro2,parametro3,lista_instrucciones);
            	break;
			}
			case F_READ:
            {
            	uint32_t parametro1, parametro2, parametro3 ;
            	if (!recv_F_READ(socket_cliente, &parametro1, &parametro2, &parametro3)) {
            	   log_error(log_kernel, "Fallo recibiendo F_READ");
            	   break;
            	}
            	cargar_instruccion3( F_READ,"F_READ",parametro1,parametro2,parametro3,lista_instrucciones);

            	break;
			}
			case DELETE_SEGMENT:
            {
            	uint32_t parametro1;

            	if (!recv_DELETE_SEGMENT(socket_cliente, &parametro1)) {
            	     log_error(log_kernel, "Fallo recibiendo DELETE_SEGMENT");
            	     break;
            	}
            	cargar_instruccion1(DELETE_SEGMENT,"DELETE_SEGMENT",parametro1,(uint32_t)NULL,lista_instrucciones);

            	break;
            }
			case F_CLOSE:
            {
            	uint32_t parametro1;

            	if (!recv_F_CLOSE(socket_cliente, &parametro1)) {
            	     log_error(log_kernel, "Fallo recibiendo F_CLOSE");
            	     break;
            	}
            	cargar_instruccion1(F_CLOSE,"F_CLOSE",parametro1,(uint32_t)NULL,lista_instrucciones);

            	break;
            }
		    case EXIT:
            {
            	cargar_instruccion2(EXIT,"EXIT",(uint32_t)NULL,(uint32_t)NULL,lista_instrucciones);
				break;
			}

            // Errores
            case -1:
                log_error(log_kernel, "Cliente desconectado de %s...", server_name);
                return;

            default:
                log_error(log_kernel, "Algo anduvo mal en el server de %s", server_name);
                log_info(log_kernel, "Cop: %d", cop);
                return;
        }
    }
    log_warning(log_kernel, "El cliente se desconecto de %s server", server_name);


    return;
}

//crea un hilo por cada cliente que se conecta
void escuchar_clientes(t_log* logger, int socket_servidor){

	args_atender_cliente* args = malloc(sizeof(args_atender_cliente));
	args->log = logger;
	args->socket = socket_servidor;
	args->server_name = "Kernel";
	while (1) {
	   pthread_t thread;
	   int *socket_cliente = malloc(sizeof(int));
	   *socket_cliente = esperar_cliente(logger, socket_servidor); //el server acepta al cliente y devuelve un socket_cliente
	   if(*socket_cliente != -1){ //si se establece la conexion
		   pthread_create(&thread, NULL, (void*) atender_cliente, (void*) args);
		   pthread_detach(thread);
	   }
	}
}
// TODO
void procesar_conexion_cpu(void* socket_cliente){

	free(socket_cliente);
}
// TODO
void procesar_conexion_fileSystem(void* socket_cliente){

	free(socket_cliente);
}
// TODO
void procesar_conexion_memoria(void* socket_cliente){

	free(socket_cliente);
}
//genero y proceso cada conexion
void generar_conexiones(t_log* logger){
	int* conexion1 = malloc(sizeof(int));
	int* conexion2 = malloc(sizeof(int));
	int* conexion3 = malloc(sizeof(int));

	pthread_t thread1, thread2, thread3;
	//creo conexiones
	*conexion1 = crear_conexion(log_kernel, "CPU", ip_cpu, puerto_cpu);
	*conexion2 = crear_conexion(log_kernel, "FileSystem", ip_fileSystem, puerto_fileSystem);
	*conexion3 = crear_conexion(log_kernel, "Memoria", ip_memoria, puerto_memoria);
	//proceso conexiones
	if(*conexion1 != -1){
		pthread_create(&thread1, NULL, (void*) procesar_conexion_cpu, conexion1);
		pthread_detach(thread1);
	}
	if(*conexion2 != -1){
		pthread_create(&thread2, NULL, (void*) procesar_conexion_fileSystem, conexion2);
		pthread_detach(thread2);
	}
	if(*conexion3 != -1){
		pthread_create(&thread3, NULL, (void*) procesar_conexion_memoria, conexion3);
		pthread_detach(thread3);
	}

	liberarConexiones(*conexion1, *conexion2, conexion3);
}

int main (){
	 	t_log* log_kernel = log_create("kernel.log", "Kernel", 1, LOG_LEVEL_DEBUG);
		t_config* config_kernel = config_create("kernel.config");
		iniciar_config(config_kernel);

		int server_fd = iniciar_servidor(log_kernel, "Kernel", ip, puerto_escucha);
		log_info(log_kernel , "Servidor listo para recibir cliente");

		//kernel se conecta a cpu, memoria y fileSystem
		generar_conexiones(log_kernel);

		//atiende los clientes y procesa las tareas de cada uno
		escuchar_clientes(log_kernel, server_fd);

		terminar_kernel(log_kernel, config_kernel);


	    return EXIT_SUCCESS;
}




