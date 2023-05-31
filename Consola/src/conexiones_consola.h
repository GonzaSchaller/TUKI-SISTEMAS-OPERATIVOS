#ifndef SRC_CONEXIONES_CONSOLA_H_
#define SRC_CONEXIONES_CONSOLA_H_

#include <stdio.h>
#include <stdlib.h>
#include <sockets.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>

extern int conexion_kernel;
extern char* ip_kernel;
extern char* puerto_kernel;

extern t_config* config_consola;
extern t_log* log_consola;
extern t_list* lista_instrucciones;


#endif /* SRC_CONEXIONES_CONSOLA_H_ */
