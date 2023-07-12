#include "comunicacion.h"
extern t_log* logger;
extern t_config_fs *c;
extern int fd_fs;
extern


void procesar_peticiones(int cliente_socket){
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
					char* nombre_archivow;
				    uint32_t dlw;
					uint32_t cbw;
					char*contenido;
					uint32_t punterow=0;
					uint32_t estadok;

					recv_F_WRITE(cliente_socket,&nombre_archivow,&dlw,&cbw); //

					log_info(logger,"Escribir Archivo: <%s> - Puntero: <%d> - Memoria <%d> - Tamanio: <%d>",nombre_archivow,punterow,dlw,cbw);

					//solicito a memoria lo que hay enla direccion logica mandada
					send_READ2(fd_memoria, dlw);
					//aca recibo el contenido que le pediu a memoria
					recv_contenido_leido(fd_memoria,&contenido);
					//recibo un puntero?

					if(escribir_contenido(contenido,punterow)){ //TODO
						estadok = EXITOSO; //escribio bien
						send_OK_CODE(cliente_socket,estadok);
					}else {
						estadok= FALLIDO; //escribio mal
						send_OK_CODE(cliente_socket,estadok);
					}

					   break;
				case F_READ:
					uint32_t estado_memoria;
					uint32_t estado_kernel;
					char* nombre_archivor;
					uint32_t df;
					uint32_t cb;
					uint32_t puntero=0;
					recv_F_READ(cliente_socket,&nombre_archivor,&df,&cb);
					log_info(logger,"Leer: Archivo: %s - Puntero: %d  - Memoria: <%d>  - Tamanio: <%d>",nombre_archivor,puntero,df,cb);
					char*contenidor = buscar_contenido(puntero,cb); //TODO
					//le mando a memoria lo que tiene que escribir
					send_WRITE(fd_memoria,df,contenidor);
					recv_OK_CODE(fd_memoria,&estado_memoria);
					if(estado_memoria == EXITOSO){
						estado_kernel = EXITOSO;
						send_OK_CODE(cliente_socket,estado_kernel);
					}else{
						estado_kernel = FALLIDO;
						send_OK_CODE(cliente_socket,estado_kernel);
					}

					   break;

				case F_TRUNCATE:

					   break;


				}

}
}


int generar_conexion_con_memoria(){
	int conexion = crear_conexion(logger,"Memoria",c->ip_memoria, c->puerto_memoria);
	uint8_t handshake =1;
	uint8_t result;
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












