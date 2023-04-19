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
//Codigo de operacions instrucciones
typedef enum{ 	//parametros
	SET,  // 2
	MOV_OUT, //2
	WAIT, //1
	IO, //1
	SIGNAL, //1
	MOV_IN, //2
	F_OPEN, //1
	YIELD,
	F_TRUNCATE,//2
	F_SEEK,//2
	CREATE_SEGMENT,//2
	F_WRITE,//3
	F_READ,//3
	DELETE_SEGMENT,//1
	F_CLOSE,//1
	EXIT
}op_code;

bool recv_SET(int socket_cliente, uint32_t* parametro1, uint32_t* parametro2);
bool recv_MOV_OUT(int socket_cliente, uint32_t* parametro1, uint32_t* parametro2);
bool recv_WAIT(int socket_cliente, uint32_t* parametro1);
bool recv_IO(int socket_cliente, uint32_t* parametro1);
bool recv_SIGNAL(int socket_cliente, uint32_t* parametro1);
bool recv_MOV_IN(int socket_cliente, uint32_t* parametro1, uint32_t* parametro2);
bool recv_F_OPEN(int socket_cliente, uint32_t* parametro);

bool recv_F_TRUNCATE(int socket_cliente, uint32_t* parametro1, uint32_t* parametro2);
bool recv_CREATE_SEGMENT(int socket_cliente, uint32_t* parametro1, uint32_t* parametro2);
bool recv_F_SEEK(int socket_cliente, uint32_t* parametro1, uint32_t* parametro2);
bool recv_F_WRITE(int socket_cliente, uint32_t* parametro1, uint32_t* parametro2, uint32_t* parametro3);
bool recv_F_READ(int socket_cliente, uint32_t* parametro1, uint32_t* parametro2, uint32_t* parametro3);
bool recv_DELETE_SEGMENT(int socket_cliente, uint32_t* parametro1);
bool recv_DELETE_SEGMENT(int socket_cliente, uint32_t*parametro1);

#endif SEND_RCVS_H_
