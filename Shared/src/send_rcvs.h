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
bool send_SET(int, uint32_t, char*);
bool recv_SET(int,uint32_t*, char**);
static void* serializar_SET(size_t*, uint32_t, char*);
static void deserializar_SET(void*,uint32_t,char**);

bool send_MOV_OUT(int, uint32_t, uint32_t);
bool recv_MOV_OUT(int, uint32_t*, uint32_t*);
static void* serializar_MOV_OUT(uint32_t , uint32_t );
static void deserializar_MOV_OUT(void* , uint32_t* , uint32_t* );


bool send_WAIT();
bool recv_WAIT();

bool send_IO(int, uint32_t);
bool recv_IO(int, uint32_t*);
static void* serializar_IO(uint32_t);
static void deserializar_IO(void* , uint32_t* );

bool send_SIGNAL();
bool recv_SIGNAL();


bool send_MOV_IN(int, uint32_t, uint32_t);
bool recv_MOV_IN(int, uint32_t*, uint32_t*);
static void* serializar_MOV_IN(uint32_t , uint32_t );
static void deserializar_MOV_IN(void* , uint32_t* , uint32_t* );

bool send_F_OPEN(int, char*);
bool recv_F_OPEN(int, char**);
static void* serializar_F_OPEN(size_t* ,char* );
static void deserializar_F_OPEN(void* ,char** );

bool send_YIELD(int);
bool recv_YIELD(int);
static void* serializar_YIELD();

bool send_F_TRUNCATE(int, char*, uint32_t);
bool recv_F_TRUNCATE(int, char**, uint32_t*);
static void* serializar_F_TRUNCATE(size_t* , char*  ,uint32_t );
static void deserializar_F_TRUNCATE(void* ,char**  , uint32_t* );

bool send_F_SEEK(int, char*, uint32_t);
bool recv_F_SEEK(int, char**, uint32_t*);

bool send_CREATE_SEGMENT(int, uint32_t, uint32_t);
bool recv_CREATE_SEGMENT(int, uint32_t*, uint32_t*);
static void* serializar_CREATE_SEGMENT(uint32_t , uint32_t );
static void deserializar_CREATE_SEGMENT(void* , uint32_t* , uint32_t* );

bool send_F_WRITE(int, char*,uint32_t, uint32_t parametro);
bool recv_F_WRITE(int, char**,uint32_t*, uint32_t*);

bool send_F_READ(int, char*,uint32_t, uint32_t);
bool recv_F_READ(int, char**,uint32_t*, uint32_t*);

bool send_DELETE_SEGMENT(int, uint32_t);
bool recv_DELETE_SEGMENT(int, uint32_t*);
static void* serializar_DELETE_SEGMENT(uint32_t );
static void deserializar_DELETE_SEGMENT(void* , uint32_t* );

bool send_F_CLOSE(int , char*);
bool recv_F_CLOSE(int , char**);

bool send_EXIT(int);
bool recv_EXIT(int);
static void* serializar_EXIT();


#endif
