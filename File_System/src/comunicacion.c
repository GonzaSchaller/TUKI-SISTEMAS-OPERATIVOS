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
					char* nombre_archivow;
				    uint32_t dlw;
					uint32_t cbw;

					recv_F_WRITE(cliente_socket, nombre_archivow,dlw, cbw);









					   break;
				case F_READ:

					char* nombre_archivo;
					uint32_t df;
					uint32_t cb;
					uint32_t puntero;
					recv_F_READ(cliente_socket, nombre_archivo,df,cb);
					//necesito recibir un punero (? o supongo que lo saco de mi fcb?
					log_info(logger,"Leer: Archivo: %s - Puntero: %d  - Memoria: <%d>  - Tamanio: <%d>",puntero,nombre_archivo,df,cb);

					char*contenido = buscar_contenido(puntero,cb);
					//le mando a memoria lo que tiene que escribir
					send_READ(fd_memoria,contenido,df);



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












