#ifndef CONSOLA_MAIN_H
#define CONSOLA_MAIN_H

#include <stdio.h>
#include <stdlib.h>

#include "../../home/utnso/tp-2023-1c-Kernel-Masters/Shared/sockets.h"


t_config* config_consola;
t_log* log_consola;
char* ip;
char* puerto;
void terminar_consola(t_log* log ,int conexion, t_config* config);

#endif
