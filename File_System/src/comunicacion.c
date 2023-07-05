#include "comunicacion.h"
extern t_log* logger;
extern t_config_fs *c;
extern int fd_fs;

static void procesar_peticiones(int cliente_socket){
	op_code cop;
	while(cliente_socket!=-1){
		if(recv(cliente_socket,&cop,sizeof(op_code),0) == 0){
			log_info(logger,"DISCONECT!");
				return;  }


				switch(cop){

				case F_OPEN:
					extra_code estado;
					char*nombre_archivo;
					uint32_t tamanio;
					if(!recv_F_OPEN(cliente_socket,&nombre_archivo)) log_error(logger, "Fallo recibiendo f_open de kernel");

					log_info(logger,"abrir archivo %s",nombre_archivo);
					bool resultado = existe_y_abrir(nombre_archivo);
					if(!resultado){
						estado = INCORRECTO;
						send_OK_CODE(cliente_socket, estado);
						//crear el arcchivo.
						recv_CREAR_ARCHIVO(cliente_socket, &nombre_archivo,&tamanio); //para mi que no deberia recibir nada

						if(crear_archivo(nombre_archivo,tamanio)){
							estado = CORRECTO;
							send_OK_CODE(cliente_socket,estado);
						}
					}
					else{
						estado = CORRECTO;
						send_OK_CODE(cliente_socket, estado);
					}

				break;


				case F_WRITE:
					uint32_t cant_bytes;
					uint32_t direccion_logica;
				//	if(!recv_F_WRITE(cliente_socket,&direccion_logica,&cant_bytes)) log_error(logger,"fallo recibiendo f_write de kernel");
					//recibir puntero?




					   break;
				case F_READ:

					uint32_t dl;
					uint32_t cb;
					//if(!recv_F_READ(cliente_socket,)) log_error(logger,"fallo recibiendo d_read de kernel");



					   break;

				case F_TRUNCATE:
					   break;


				}

}
}


int generar_conexion_con_memoria(){
	int conexion = crear_conexion(logger,"Memoria",c->ip_memoria, c->puerto_memoria);
	uint32_t handshake =1;
	uint32_t result;
	send_handshake(conexion,handshake);
	recv_handshake(conexion,&result);
	if(result == 1) log_info(logger,"todo ok capo");
	return conexion;
}


void conexion_kernel(){
	int fd_kernel = esperar_cliente(logger, fd_fs);
		if(fd_kernel!=-1){
			procesar_peticiones(fd_kernel);
		}
}












