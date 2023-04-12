#ifndef KERNEL_MAINH
#define KERNEL_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <../../home/utnso/tp-2023-1c-Kernel-Masters/Shared/sockets.h>


t_log* log_kernel;
char* ip = "127.0.0.1";
char* puerto_escucha;

char* ip_cpu;
char* puerto_cpu;
char* ip_memoria;
char* puerto_memoria;
char* ip_fileSystem;
char* puerto_fileSystem;
char* algoritmo_planificacion;
uint32_t estimacion_inicial;

uint32_t grado_max_multiprogramacion;


#endif
