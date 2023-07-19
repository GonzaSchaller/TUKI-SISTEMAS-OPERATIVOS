#ifndef INSTRUCCIONES_H
#define INSTRUCCIONES_H

#include <stdint.h> // pasarlo a todos los archivos
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>

#include <sockets.h>
#include <sys/socket.h>

#include <signal.h>
#include <unistd.h>
#include <netdb.h>

#include "send_rcvs.h"
#include "estructuras_compartidas.h"
#include "recibo_instrucciones.h"
#include "mmu.h"
//#include "cpu.h" //para agregar conexion_memoria

extern int socket_cliente_kernel; //ver como agregar aca el socket kernel
extern int socket_memoria;
extern uint32_t seguir_ejecutando;
typedef struct{
    uint32_t PID;
    uint32_t PC;
    t_list* instrucciones;
    registros_cpu registros;
    t_list* TSegmento;
}pcb_cpu;

void pasar_al_registro(char, char);
void set_socket_kernel(int);
void ejecutar_SET(pcb_cpu*, uint32_t, char*);
int ejecutar_MOV_IN(pcb_cpu*, uint32_t, uint32_t);
int ejecutar_MOV_OUT(pcb_cpu*, uint32_t, uint32_t);
int ejecutar_IO(pcb_cpu*, uint32_t);
int ejecutar_F_OPEN(pcb_cpu*, char*);
int ejecutar_F_CLOSE(pcb_cpu*, char*);
int ejecutar_F_SEEK(pcb_cpu*, char*, uint32_t);
int ejecutar_F_READ(pcb_cpu*, char*, uint32_t, uint32_t);
int ejecutar_F_WRITE(pcb_cpu*, char*, uint32_t, uint32_t);
int ejecutar_F_TRUNCATE(pcb_cpu*, char*, uint32_t);
int ejecutar_WAIT(pcb_cpu*, char*);
int ejecutar_SIGNAL(pcb_cpu*, char*);
int ejecutar_CREATE_SEGMENT(pcb_cpu*, uint32_t, uint32_t);
int ejecutar_DELETE_SEGMENT(pcb_cpu*, uint32_t);
int ejecutar_YIELD(pcb_cpu*);
int ejecutar_EXIT(pcb_cpu*);
void obtener_dir_fisica(uint32_t , t_list* , uint32_t*, uint32_t*, uint32_t*, uint32_t*, uint32_t *);
void segmentation_fault(pcb_cpu*, uint32_t, uint32_t, uint32_t, uint32_t);
#endif
