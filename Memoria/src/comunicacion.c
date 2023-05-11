#include "comunicacion.h"
#include <send_rcvs.h>

extern t_log*log_memoria;
extern t_segmento;

//conexiones de kernel, cpu, filesystem
typedef struct{
	int fd;
	char*server_name;
} t_procesar_conexion_args;

static void atender_cliente(){
	printf("hola");
}
//esquema de memoria: segmentacion

/*
static void procesar_conexion(void* void_args){
	t_procesar_conexion_args*args = (t_procesar_conexion_args*) void_args;
	int cliente_socket = args->fd;
	char*server_name = args->server_name;
	free(args);

	op_code cop;
	while(cliente_socket!=-1){

		//0if(recv(cliente_socket,&cop,sizeof(op_code),0) == 0){
		//	log_info(logger,"DISCONECT!");
		//	return;
		//}

		switch(cop){
		case CREATE_SEGMENT:
			//--------CONEXION KERNEL-MEMORIA-------
			//CREACION DE SEGMENTO CREATE-SEGMENT
			//uno se me conecta el kernel y me manda un mensaje para que le cree un segmento con tamanio definido
			//entonces pueden ocurrir 3 cosas:
		//1) el segmento se crea exitosamente y le devuelvo la base del nuevo segmento // si hay espacio de manera contigua, creo el segmento y le mando la direccion.
		//2) no hay mas espacio dispoible en la memoria y por lo tanto el proceso finaliza con un out of memory // decirle que no tengo el espacio libre.
		//3) tengo espacio disponible pero no esta contiguo, por lo que tengo que decir que se tiene que compactar. // le informo al kernel que se debe solicitar una compactacion previa a crear el segmento.

			break;

		case DELETE_SEGMENT:

			// el kernel tiene que hacer un DELETE SEGMENT, me manda el id del segmento a eliminar, voy lo elimino, y le devuelvo la tabla de segmentos actualizada.
		//Marco el segmento como libre y en caso de que tenga huecos libres aledanios, los debo consolidar actializando sus estructuras administrativas.


			break;

		case F_WRITE:
		// - pedido de escritura: escribir lo pedido en la pos pedida y responder un mensaje de ok
		//cada acceso a memoria tiene un tiempo de espera en milisegundos definido por archivo de config.
			break;

		case F_READ:
		// - pedido de lectura: devolver el valor que se encuentra en la pos pedida.
			break;

		default:
			//log_error(logger,"algo salio mal");


		// ALGUIEN ME AVISA QUE FINALIZO UN PROCESO Y LIBERO SU ESPACIO DE MEMORIA.


		//---CONEXION MEMORIA - CPU --------- READ / WRITE
		//encargado de responder los pedidos realizados por la CPU para leer y/o escribir en los segmentos de datos del proceso en ejecucion, referenciados por diversas tablas de paginas
		//puede pedirme

		//CONEXION CPU-FILESYSTEM-MEMORIA : me dan una direccion fisica, y solicitan accesos al espacio de usuario de memoria.

		//COMPACTACION DE SEGMENTOS:
		//debo mover los segmentos a fin de eleiminar los guecos libres entre los mismos, dejando un unico huevo libre de todo el espacio disponible.
		// tarea principal de esta operacion: informar al kernel las tablas de segmentos actualizadas.



				//TODO
		}

	}

	//log_warning(logger,"el cliente se desconexto del server\n");

}
*/

int server_escuchar1(t_log*logger,int server_socket)
{
	   int socket_cliente = esperar_cliente(logger,server_socket);

	   if(socket_cliente != -1){

	   pthread_t thread;

	   pthread_create(&thread, NULL, (void*)atender_cliente, socket_cliente);
	   pthread_detach(thread);
	   }
return 0;
}

/*
int server_escuchar1(t_log*logger,int server_socket)
{
	for (int i=0;i<3;i++){
		int socket_cliente = esperar_cliente(logger,server_socket);
		if (cliente_socket != -1) {
		        pthread_t hilo;
		        t_procesar_conexion_args* args = malloc(sizeof(t_procesar_conexion_args));
		        args->fd = cliente_socket;
		        args->server_name = server_name;
		        pthread_create(&hilo, NULL, (void*) procesar_conexion, (void*) args);
		        pthread_detach(hilo);
		        return 1;
		    }

		i++;

	}

*/

return 0;
}


/*
int server_escuchar2(t_log*logger, char*server_name, int server_socket){
	int cliente_socket = esperar_cliente2(logger,server_name,server_socket);
	if(cliente_socket != -1){
		pthread_t hilo;
		t_procesar_conexion_args* args = malloc(sizeof(t_procesar_conexion_args));
		args->fd = cliente_socket;
		args->server_name = server_name;
		pthread_create(&hilo,NULL,(void*)procesar_conexion,(void*)args);
		pthread_detach(hilo);
		return 1;
	}
	return 0;
}

*/




static void procesar_conexion(void* void_args){
	t_procesar_conexion_args*args = (t_procesar_conexion_args*) void_args;
	int cliente_socket = args->fd;
	char*server_name = args->server_name;
	free(args);


	op_code cop;
		while(cliente_socket!=-1){
			if(recv(cliente_socket,&cop,sizeof(op_code),0) == 0){
				log_info(log_memoria,"DISCONECT!");
				return;
			}

			switch(cop){
		//	case HANDSHAKE:



				break;

			case CREATE_SEGMENT:

				crear_segmento();

				break;

			case DELETE_SEGMENT:
				borrar_segmento();

				break;


}


int server_escuchar(t_log* log_memoria,char* server_name, int server_socket) {

    int cliente_socket = esperar_cliente(log_memoria, server_socket);

    if (cliente_socket != -1) {
        pthread_t hilo;
        t_procesar_conexion_args* args = malloc(sizeof(t_procesar_conexion_args));
        args->fd = cliente_socket;
        args->server_name = server_name;
        pthread_create(&hilo, NULL, (void*) procesar_conexion, (void*) args);
        pthread_detach(hilo);
        return 1;
    }
    else log_info(log_memoria,"no ai");
    return 0;
}

int server_escuchar3(t_log* log_memoria,char* server_name, int server_socket) {

	for(int i=0;i<3;i++){
		int cliente_socket = esperar_cliente(log_memoria, server_socket);

		if (cliente_socket != -1) {
		        pthread_t hilo;
		        t_procesar_conexion_args* args = malloc(sizeof(t_procesar_conexion_args));
		        args->fd = cliente_socket;
		        args->server_name = server_name;
		        pthread_create(&hilo, NULL, (void*) procesar_conexion, (void*) args);
		        pthread_detach(hilo);
		        return 1;
		    }
		    i++;
	}
    return 0;
}
