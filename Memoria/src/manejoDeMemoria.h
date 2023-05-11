manejodememorr.h


#ifndef MANEJO_DE_MEMORIA_H
#define MANEJO_DE_MEMORIA_H


#include <ctype.h>//
#include <stdint.h>
#include <stdio.h>//
#include <stdlib.h>//
#include <stdbool.h>//
#include <pthread.h>//
#include <commons/log.h> //
#include <commons/string.h>//
#include <commons/config.h>//
#include <commons/collections/list.h>//
#include <commons/collections/queue.h>//
#include <readline/readline.h>
#include <assert.h>//
#include <sockets.h>//
#include<signal.h>//
#include<unistd.h>//
#include<sys/socket.h>//
#include<netdb.h>//
#include<string.h>//

#include "iniciar.h"
#include "comunicacion.h"
#include <estructuras_compartidas.h>



void crear_segmento(int cliente_fd);
void borrar_segmento(int cliente_fd);



#endif
