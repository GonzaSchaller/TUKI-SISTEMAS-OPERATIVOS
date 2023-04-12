#ifndef KERNEL_H
#define KERNEL_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include "sockets.h"

int iniciar_servidor (t_log* , char* , char* );
int esperar_cliente(t_log* ,int);
int crear_conexicon(t_log* , char* , char* );

#endif
