#ifndef SEND_RCVS_H_
#define SEND_RCVS_H_

#include <inttypes.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "estructuras_compartidas.h"
#include <commons/collections/list.h>
//a ver ah


bool send_SET(int, uint32_t, char*);
bool recv_SET(int,uint32_t*, char**);

bool send_MOV_OUT(int, uint32_t, uint32_t);
bool recv_MOV_OUT(int, uint32_t*, uint32_t*);

bool send_WAIT(int socket_cliente, char* recurso);
bool recv_WAIT(int socket_cliente, char** recurso);

bool send_IO(int, uint32_t);
bool recv_IO(int, uint32_t*);

bool send_SIGNAL(int socket_cliente, char* recurso);
bool recv_SIGNAL(int socket_cliente, char** recurso);


bool send_MOV_IN(int, uint32_t, uint32_t);
bool recv_MOV_IN(int, uint32_t*, uint32_t*);


bool send_F_OPEN(int, char*);
bool recv_F_OPEN(int, char**);


bool send_YIELD(int);
bool recv_YIELD(int);


bool send_F_TRUNCATE(int, char*, uint32_t);
bool recv_F_TRUNCATE(int, char**, uint32_t*);


bool send_F_SEEK(int, char*, uint32_t);
bool recv_F_SEEK(int, char**, uint32_t*);

bool send_CREATE_SEGMENT(int, uint32_t, uint32_t);
bool recv_CREATE_SEGMENT(int, uint32_t*, uint32_t*);


bool send_F_WRITE(int, char*,uint32_t, uint32_t );
bool recv_F_WRITE(int, char**,uint32_t*, uint32_t*);

bool send_F_READ(int, char*,uint32_t, uint32_t);
bool recv_F_READ(int, char**,uint32_t*, uint32_t*);

bool send_DELETE_SEGMENT(int, uint32_t);
bool recv_DELETE_SEGMENT(int, uint32_t*);


bool send_F_CLOSE(int , char*);
bool recv_F_CLOSE(int , char**);

bool send_EXIT(int);
bool recv_EXIT(int);

void send_INICIAR_ESTRUCTURA_MEMORIA(int );
bool recv_INICIAR_ESTRUCTURA_MEMORIA(int);

bool send_SEGMENTO(int, segmento_t*);
bool recv_SEGMENTO(int, segmento_t** );

bool send_PID(int,uint32_t);
bool recv_PID(int, uint32_t*);

bool send_PC(int, uint32_t);
bool recv_PC(int, uint32_t*);

bool send_tiempo_bloqueante(int, float);
bool recv_tiempo_bloqueante(int, float*);

bool send_BASE_SEGMENTO(int, uint32_t);
bool recv_BASE_SEGMENTO(int, uint32_t*);

bool recv_TABLA_SEGMENTOS(int ,t_list**);
bool send_TABLA_SEGMENTOS(int ,t_list* );

bool recv_ID_SEGMENTO(int, uint32_t*);
bool send_ID_SEGMENTO(int, uint32_t );

bool send_CANT_INSTRUCCIONES(int, uint32_t);
bool recv_CANT_INSTRUCCIONES(int, uint32_t*);

bool send_REG_CPU(int, registros_cpu );
bool recv_REG_CPU(int, registros_cpu* );

bool send_CONTEXTO_EJECUCION(int, contexto_ejecucion);
bool recv_CONTEXTO_EJECUCION(int , contexto_ejecucion*);


//void send_CONTEXTO_EJECUCION(int,contexto_ejecucion);
//void recv_CONTEXTO_EJECUCION(int, contexto_ejecucion*);
bool send_handshake(int,uint32_t );
bool recv_handshake(int ,uint32_t*);


bool send_FINALIZAR_ESTRUCTURAS(int);
bool recv_FINALIZAR_ESTRUCTURAS(int);

bool send_EXISTE_ARCHIVO(int socket_cliente, char* );
bool recv_EXISTE_ARCHIVO(int socket_cliente, char** );

bool send_OK_CODE(int, extra_code );
bool recv_OK_CODE(int, extra_code*);

bool send_CREAR_ARCHIVO(int, char* ,uint32_t );
bool recv_CREAR_ARCHIVO(int, char**,uint32_t*);


bool send_FINALIZAR_TRUNCATE(int, uint32_t);
bool recv_FINALIZAR_TRUNCATE(int, uint32_t*);

bool send_FINALIZAR_READ(int,uint32_t);
bool recv_FINALIZAR_READ(int,uint32_t*);

bool send_FINALIZAR_WRITE(int,uint32_t );
bool recv_FINALIZAR_WRITE(int,uint32_t* );

//bool recv_READ2(int ,uint32_t* ,char** );
//bool send_READ2(int ,uint32_t  , char* ); //////Dddd

//bool recv_READ(int , uint32_t* ,uint32_t* );
//bool send_READ(int , uint32_t  ,uint32_t  );

//bool recv_WRITE(int ,uint32_t* ,char** );
//bool send_WRITE(int ,uint32_t  , char* );
bool send_WRITE_CPU(int fd,uint32_t parametro1 , char* parametro2);
bool recv_WRITE_CPU(int fd,uint32_t* parametro1,char** parametro2);


bool send_WRITE_FS(int fd,uint32_t parametro1 , char* parametro2);
bool recv_WRITE_FS(int fd,uint32_t* parametro1,char** parametro2);

bool send_READ_CPU(int socket_cliente, uint32_t  parametro1,uint32_t  parametro2);
bool recv_READ_CPU(int socket_cliente, uint32_t* parametro1,uint32_t* parametro2);

bool send_READ_FS(int socket_cliente, uint32_t  parametro1,uint32_t  parametro2);
bool recv_READ_FS(int socket_cliente, uint32_t* parametro1,uint32_t* parametro2);




bool recv_direccion_fisica(int , uint32_t* );
bool send_direccion_fisica (int , uint32_t  );

bool recv_tamanio_a_leer(int , uint32_t* );
bool send_tamaio_a_leer(int , uint32_t  );

bool recv_cant_bytes(int , uint32_t* );
bool send_cant_bytes(int , uint32_t  );

bool recv_contenido_leido(int, char** );
bool send_contenido_leido(int , char* );

bool recv_EXISTE_ARCHIVO2(int , char** );
bool send_EXISTE_ARCHIVO2(int , char* );

bool recv_PUNTERO_FS(int, uint32_t* );
bool send_PUNTERO_FS(int, uint32_t );

bool recv_ERROR(int );
bool send_ERROR(int );

bool recv_LEER_CONTENIDO_CPU(int);
bool send_LEER_CONTENIDO_CPU(int);

uint32_t recv_seguir_ejecutando(int);
bool send_seguir_ejecutando(int , uint32_t  );

#endif
