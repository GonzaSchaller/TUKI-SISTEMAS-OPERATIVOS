#include "send_rcvs.h"
static void* serializar_SET(size_t* size, char* parametro2, uint32_t parametro1) {
	// Carga los parametros al reves pero funciona como deberia funcionar
	size_t size_parametro2 = strlen(parametro2) + 1;
    *size =
          sizeof(op_code)   // cop
        + sizeof(size_t)    // total
        + sizeof(size_t)    // size de char* parametro2
        + size_parametro2         // char* parametro2
        + sizeof(uint32_t);  // parametro1
    size_t size_payload = *size - sizeof(op_code) - sizeof(size_t);
    void* stream = malloc(*size);
    op_code cop = SET;
    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream+sizeof(op_code), &size_payload, sizeof(size_t));
    memcpy(stream+sizeof(op_code)+sizeof(size_t), &size_parametro2, sizeof(size_t));
    memcpy(stream+sizeof(op_code)+sizeof(size_t)*2, parametro2, size_parametro2);
    memcpy(stream+sizeof(op_code)+sizeof(size_t)*2+size_parametro2, &parametro1, sizeof(uint32_t));

    return stream;
}

static void deserializar_SET(void* stream, char** parametro2, uint32_t* parametro1) {
	// Carga los parametros al reves pero funciona como deberia funcionar
	size_t size_parametro2;
    memcpy(&size_parametro2, stream, sizeof(size_t));

    char* p2 = malloc(size_parametro2);
    memcpy(p2, stream+sizeof(size_t), size_parametro2);
    *parametro2 = p2;

    memcpy(parametro1, stream+sizeof(size_t)+size_parametro2, sizeof(uint32_t));
}

bool recv_SET(int fd,uint32_t* parametro1,char** parametro2) {

    size_t size_payload;
    if (recv(fd, &size_payload, sizeof(size_t), 0) != sizeof(size_t))
        return false;

    void* stream = malloc(size_payload);
    if (recv(fd, stream, size_payload, 0) != size_payload) {
        free(stream);
        return false;
    }

    deserializar_SET(stream, parametro2, parametro1);

    free(stream);
    return true;
}

bool send_SET(int fd,uint32_t parametro1 , char* parametro2) {
    size_t size;
    void* stream = serializar_SET(&size, parametro2, parametro1);
    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

static void* serializar_MOV_OUT(uint32_t parametro1, uint32_t parametro2) {
    void* stream = malloc(sizeof(op_code) + sizeof(uint32_t) * 2);
    op_code cop = MOV_OUT;
    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream+sizeof(op_code), &parametro1, sizeof(uint32_t));
    memcpy(stream+sizeof(op_code)+sizeof(uint32_t), &parametro2, sizeof(uint32_t));
    return stream;
}
static void deserializar_MOV_OUT(void* stream, uint32_t* parametro1, uint32_t* parametro2) {
    memcpy(parametro1, stream, sizeof(uint32_t));
    memcpy(parametro2, stream+sizeof(uint32_t), sizeof(uint32_t));
}
bool recv_MOV_OUT(int socket_cliente, uint32_t* parametro1, uint32_t* parametro2){
    size_t size = sizeof(uint32_t) * 2;
    void* stream = malloc(size);
    if (recv(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    deserializar_MOV_OUT(stream, parametro1, parametro2);
    free(stream);
	return true;
}
bool send_MOV_OUT(int socket_cliente, uint32_t  parametro1, uint32_t  parametro2){
    size_t size = sizeof(op_code) + sizeof(uint32_t) * 2;
    void* stream = serializar_MOV_OUT(parametro1, parametro2);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

static void* serializar_WAIT(size_t* size,char* recurso){
		size_t size_recurso = strlen(recurso)+1;
		*size =sizeof(op_code) + 2*sizeof(size_t)+ size_recurso;
		size_t size_payload = *size - sizeof(op_code) - sizeof(size_t);
		void* stream = malloc(*size);
		op_code cop = WAIT;
		    memcpy(stream, &cop, sizeof(op_code));
		    memcpy(stream+sizeof(op_code), &size_payload, sizeof(size_t));
		    memcpy(stream+sizeof(op_code)+sizeof(size_t), &size_recurso, sizeof(size_t));
		    memcpy(stream+sizeof(op_code)+sizeof(size_t)+sizeof(size_t), recurso, size_recurso);
	// copio al stream en orden cop,payload,sizep2,p2
		    return stream;
}
static void deserializar_WAIT(void* stream,char** recurso){
	size_t size_archivo;
	memcpy(&size_archivo, stream, sizeof(size_t));
	char* p2 = malloc(size_archivo);
	*recurso = p2;
	memcpy(p2,stream+sizeof(size_t) ,size_archivo);
// OPCODE,PAYLOAD, SIZE P2, P2
}
bool recv_WAIT(int socket_cliente, char** recurso){ //
	 size_t size_payload ;
	    if (recv(socket_cliente,&size_payload,sizeof(size_t), 0) != sizeof(size_t)) {
	        return false;
	    }
	    void* stream = malloc(size_payload);
	    if (recv(socket_cliente,stream ,size_payload, 0) != size_payload) {
	        free(stream);
	        return false;
	    }
	    deserializar_WAIT(stream, recurso);
	    free(stream);
	    return true;
}
bool send_WAIT(int socket_cliente, char* recurso){ //
    size_t size;
    void* stream = serializar_WAIT(&size ,recurso);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

static void* serializar_IO(uint32_t parametro1) {
   void* stream = malloc(sizeof(op_code) + sizeof(uint32_t));
   op_code cop = IO;
    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream+sizeof(op_code), &parametro1, sizeof(uint32_t));
    return stream;
}
static void deserializar_IO(void* stream, uint32_t* parametro1) {
    memcpy(parametro1, stream ,sizeof(uint32_t));
}
bool recv_IO(int socket_cliente, uint32_t* parametro1){
    size_t size = sizeof(uint32_t);
    void* stream = malloc(size);
    if (recv(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    deserializar_IO(stream, parametro1);
    free(stream);
	return true;
}
bool send_IO(int socket_cliente, uint32_t  parametro1){
    size_t size = sizeof(op_code) + sizeof(uint32_t);
    void* stream = serializar_IO(parametro1);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

static void* serializar_SIGNAL(size_t* size,char* recurso){
		size_t size_recurso = strlen(recurso)+1;
		*size =sizeof(op_code) + 2*sizeof(size_t)+ size_recurso;
		size_t size_payload = *size - sizeof(op_code) - sizeof(size_t);
		void* stream = malloc(*size);
		op_code cop = SIGNAL;
		    memcpy(stream, &cop, sizeof(op_code));
		    memcpy(stream+sizeof(op_code), &size_payload, sizeof(size_t));
		    memcpy(stream+sizeof(op_code)+sizeof(size_t), &size_recurso, sizeof(size_t));
		    memcpy(stream+sizeof(op_code)+sizeof(size_t)+sizeof(size_t), recurso, size_recurso);
	// copio al stream en orden cop,payload,sizep2,p2
		    return stream;

}
static void deserializar_SIGNAL(void* stream,char** recurso){
	size_t size_archivo;
	memcpy(&size_archivo, stream, sizeof(size_t));
	char* p2 = malloc(size_archivo);
	*recurso = p2;
	memcpy(p2,stream+sizeof(size_t) ,size_archivo);
// OPCODE,PAYLOAD, SIZE P2, P2
}
bool recv_SIGNAL(int socket_cliente, char** recurso){
	 size_t size_payload ;
	    if (recv(socket_cliente,&size_payload,sizeof(size_t), 0) != sizeof(size_t)) {
	        return false;
	    }
	    void* stream = malloc(size_payload);

	    if (recv(socket_cliente,stream ,size_payload, 0) != size_payload) {
	        free(stream);
	        return false;
	    }
	    deserializar_SIGNAL(stream, recurso);
	    free(stream);
	    return true;
}
bool send_SIGNAL(int socket_cliente, char* recurso){
    size_t size;
    void* stream = serializar_SIGNAL(&size ,recurso);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

static void* serializar_MOV_IN(uint32_t parametro1, uint32_t parametro2) {
    void* stream = malloc(sizeof(op_code) + sizeof(uint32_t) * 2);
    op_code cop = MOV_IN;
    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream+sizeof(op_code), &parametro1, sizeof(uint32_t));
    memcpy(stream+sizeof(op_code)+sizeof(uint32_t), &parametro2, sizeof(uint32_t));


    return stream;
}
static void deserializar_MOV_IN(void* stream, uint32_t* parametro1, uint32_t* parametro2) {
    memcpy(parametro1, stream, sizeof(uint32_t));
    memcpy(parametro2, stream+sizeof(uint32_t), sizeof(uint32_t));
}
bool recv_MOV_IN(int socket_cliente, uint32_t* parametro1,uint32_t* parametro2){
    size_t size = sizeof(uint32_t) * 2;
    void* stream = malloc(size);
    if (recv(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    deserializar_MOV_IN(stream, parametro1, parametro2);
    free(stream);
    return true;

}
bool send_MOV_IN(int socket_cliente, uint32_t  parametro1,uint32_t  parametro2){
    size_t size = sizeof(op_code) + sizeof(uint32_t) * 2;
    void* stream = serializar_MOV_IN(parametro1, parametro2);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

static void* serializar_F_OPEN(size_t* size,char* archivo){
		size_t size_archivo = strlen(archivo)+1; // No sabemos si agregar +1 PREGUNTAR
		*size =sizeof(op_code) + 2*sizeof(size_t)+ size_archivo;
		size_t size_payload = *size - sizeof(op_code) - sizeof(size_t);
		void* stream = malloc(*size);
		op_code cop = F_OPEN;
		    memcpy(stream, &cop, sizeof(op_code));
		    memcpy(stream+sizeof(op_code), &size_payload, sizeof(size_t));
		    memcpy(stream+sizeof(op_code)+sizeof(size_t), &size_archivo, sizeof(size_t));
		    memcpy(stream+sizeof(op_code)+sizeof(size_t)+size_archivo, archivo, size_archivo);
	// copio al stream en orden cop,payload,sizep2,p2
		    return stream;
}
static void deserializar_F_OPEN(void* stream,char** archivo){
	size_t size_archivo;
	memcpy(&size_archivo, stream, sizeof(size_t));
	char* p2 = malloc(size_archivo);
	*archivo = p2;
	memcpy(p2,stream+sizeof(size_t) ,size_archivo);
// OPCODE,PAYLOAD, SIZE P2, P2
}
bool recv_F_OPEN(int socket_cliente, char** archivo){
	 size_t size_payload ;

	    if (recv(socket_cliente,&size_payload,sizeof(size_t), 0) != sizeof(size_t)) {
	        return false;
	    }
	    void* stream = malloc(size_payload);

	    if (recv(socket_cliente,stream ,size_payload, 0) != size_payload) {
	        free(stream);
	        return false;
	    }
	    deserializar_F_OPEN(stream, archivo);
	    free(stream);
	    return true;
}
bool send_F_OPEN(int socket_cliente, char* archivo){
    size_t size;
    void* stream = serializar_F_OPEN(&size ,archivo);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

static void* serializar_YIELD() {
   void* stream = malloc(sizeof(op_code));
    op_code cop = YIELD;
    memcpy(stream, &cop, sizeof(op_code));
    return stream;
}
bool recv_YIELD(int socket_cliente){

	    size_t size = sizeof(op_code);
	    void* stream = malloc(size);

	    if (recv(socket_cliente, stream, size, 0) != size) {
	        free(stream);
	        return false;
	    }
	    //deserializar_YIELD(stream);
	    free(stream);
	    return true;

}
bool send_YIELD(int socket_cliente){
	op_code instruccion = YIELD;
   size_t size = sizeof(op_code);
    void* stream = serializar_YIELD(instruccion);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    free(stream);
    return true;
}

static void* serializar_F_TRUNCATE(size_t* size, char* parametro1 ,uint32_t  parametro2 ){
	size_t size_parametro1 = strlen(parametro1) +1; // No sabemos si agregar +1 PREGUNTAR
	*size =sizeof(op_code) + 2*sizeof(size_t) + sizeof(uint32_t)+ size_parametro1;
	size_t size_payload = *size - sizeof(op_code) - sizeof(size_t);
	void* stream = malloc(*size);
	op_code cop = F_TRUNCATE;
	    memcpy(stream, &cop, sizeof(op_code));
	    memcpy(stream+sizeof(op_code), &size_payload, sizeof(size_t));
	    memcpy(stream+sizeof(op_code)+sizeof(size_t), &size_parametro1,sizeof(size_t));
	    memcpy(stream+sizeof(op_code)+sizeof(size_t)+sizeof(size_t), parametro1, size_parametro1);
	    memcpy(stream+sizeof(op_code)+sizeof(size_t)+sizeof(size_t) + size_parametro1, &parametro2, sizeof(uint32_t));
// copio al stream en orden cop,payload,sizep1,p1,p2
	    return stream;
}
static void deserializar_F_TRUNCATE(void* stream,char** parametro1 , uint32_t* parametro2){
	size_t size_parametro1;
	memcpy(&size_parametro1, stream, sizeof(size_t));
	char* p1 = malloc(size_parametro1);
	memcpy(p1,stream+sizeof(size_t) ,size_parametro1);
	*parametro1 = p1;
	memcpy(parametro2,stream+sizeof(size_t)+size_parametro1,sizeof(uint32_t));
// OPCODE,PAYLOAD, SIZE P2, P2,ENTERO P1,
}

bool recv_F_TRUNCATE(int socket_cliente, char** parametro1,uint32_t* parametro2){
    size_t size_payload ;
    if (recv(socket_cliente,&size_payload,sizeof(size_t), 0) != sizeof(size_t)) {
        return false;
    }
    void* stream = malloc(size_payload);

    if (recv(socket_cliente,stream ,size_payload, 0) != size_payload) {
        free(stream);
        return false;
    }
    deserializar_F_TRUNCATE(stream, parametro1, parametro2);
    free(stream);
    return true;
}
bool send_F_TRUNCATE(int socket_cliente, char*  parametro1,uint32_t  parametro2){
    size_t size;
    void* stream = serializar_F_TRUNCATE(&size ,parametro1, parametro2);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

static void* serializar_F_SEEK(size_t* size, char* parametro1, uint32_t  parametro2 ){
	size_t size_parametro1 = strlen(parametro1) +1; // No sabemos si agregar +1 PREGUNTAR
	*size =sizeof(op_code) + 2*sizeof(size_t) + sizeof(uint32_t)+ size_parametro1;
	size_t size_payload = *size - sizeof(op_code) - sizeof(size_t);
	void* stream = malloc(*size);
	op_code cop = F_SEEK;
	    memcpy(stream, &cop, sizeof(op_code));
	    memcpy(stream+sizeof(op_code), &size_payload, sizeof(size_t));
	    memcpy(stream+sizeof(op_code)+sizeof(size_t), &size_parametro1,sizeof(size_t));
	    memcpy(stream+sizeof(op_code)+sizeof(size_t)+sizeof(size_t), parametro1, size_parametro1);
	    memcpy(stream+sizeof(op_code)+sizeof(size_t)+sizeof(size_t) + size_parametro1, &parametro2, sizeof(uint32_t));
// copio al stream en orden cop,payload,sizep1,p1,p2
	    return stream;
}
static void deserializar_F_SEEK(void* stream,char** parametro1 ,uint32_t* parametro2){
	size_t size_parametro1;
		memcpy(&size_parametro1, stream, sizeof(size_t));
		char* p1 = malloc(size_parametro1);
		memcpy(p1,stream+sizeof(size_t) ,size_parametro1);
		*parametro1 = p1;
		memcpy(parametro2,stream+sizeof(size_t)+size_parametro1,sizeof(uint32_t));
}
bool recv_F_SEEK(int socket_cliente, char** parametro1,uint32_t* parametro2){
    size_t size_payload ;
    if (recv(socket_cliente,&size_payload,sizeof(size_t), 0) != sizeof(size_t)) {
        return false;
    }
    void* stream = malloc(size_payload);
    if (recv(socket_cliente,stream ,size_payload, 0) != size_payload) {
        free(stream);
        return false;
    }
    deserializar_F_SEEK(stream, parametro1, parametro2);
    free(stream);
    return true;
}
bool send_F_SEEK(int socket_cliente, char*  parametro1,uint32_t  parametro2){
	  size_t size;
	   void* stream = serializar_F_SEEK(&size ,parametro1, parametro2);
	   if (send(socket_cliente, stream, size, 0) != size) {
	       free(stream);
	       return false;
	   }
	   free(stream);
	   return true;
}

static void* serializar_CREATE_SEGMENT(uint32_t parametro1, uint32_t parametro2) {
    void* stream = malloc(sizeof(op_code) + sizeof(uint32_t) * 2);
    op_code cop = CREATE_SEGMENT;
    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream+sizeof(op_code), &parametro1, sizeof(uint32_t));
    memcpy(stream+sizeof(op_code)+sizeof(uint32_t), &parametro2, sizeof(uint32_t));
    return stream;
}
static void deserializar_CREATE_SEGMENT(void* stream, uint32_t* parametro1, uint32_t* parametro2) {
    memcpy(parametro1, stream, sizeof(uint32_t));
    memcpy(parametro2, stream+sizeof(uint32_t), sizeof(uint32_t));
}
bool recv_CREATE_SEGMENT(int socket_cliente, uint32_t* parametro1,uint32_t* parametro2){
	 size_t size = sizeof(uint32_t) * 2;
	    void* stream = malloc(size);
	    if (recv(socket_cliente, stream, size, 0) != size) {
	        free(stream);
	        return false;
	    }
	    deserializar_CREATE_SEGMENT(stream, parametro1, parametro2);
	    free(stream);
		return true;
}
bool send_CREATE_SEGMENT(int socket_cliente, uint32_t  parametro1,uint32_t  parametro2){
    size_t size = sizeof(op_code) + sizeof(uint32_t) * 2;
    void* stream = serializar_CREATE_SEGMENT(parametro1, parametro2);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

static void* serializar_F_WRITE(size_t* size, char* parametro1 ,uint32_t  parametro2 ,uint32_t parametro3){
	size_t size_parametro1 = strlen(parametro1) +1; // No sabemos si agregar +1 PREGUNTAR
	*size =sizeof(op_code) + 2*sizeof(size_t) + sizeof(uint32_t)+ size_parametro1;
	size_t size_payload = *size - sizeof(op_code) - sizeof(size_t);
	void* stream = malloc(*size);
	op_code cop = F_WRITE;
	    memcpy(stream, &cop, sizeof(op_code));
	    memcpy(stream+sizeof(op_code), &size_payload, sizeof(size_t));
	    memcpy(stream+sizeof(op_code)+sizeof(size_t), &size_parametro1,sizeof(size_t));
	    memcpy(stream+sizeof(op_code)+sizeof(size_t)+sizeof(size_t), parametro1, size_parametro1);
	    memcpy(stream+sizeof(op_code)+sizeof(size_t)+sizeof(size_t) + size_parametro1, &parametro2, sizeof(uint32_t));
	    memcpy(stream+sizeof(op_code)+sizeof(size_t)+sizeof(size_t) + size_parametro1+sizeof(uint32_t), &parametro3, sizeof(uint32_t));
// copio al stream en orden cop,payload,sizep1,p1,p2,p3
	    return stream;
}
static void deserializar_F_WRITE(void* stream,char** parametro1 , uint32_t* parametro2,uint32_t* parametro3){
	size_t size_parametro1;
	memcpy(&size_parametro1, stream, sizeof(size_t));
	char* p1 = malloc(size_parametro1);
	memcpy(p1,stream+sizeof(size_t) ,size_parametro1);
	*parametro1 = p1;
	memcpy(parametro2,stream+sizeof(size_t)+size_parametro1,sizeof(uint32_t));
	memcpy(parametro3,stream+sizeof(size_t)+size_parametro1+sizeof(uint32_t),sizeof(uint32_t));
// OPCODE,PAYLOAD, SIZE P2, P2,ENTERO P1,
}
bool recv_F_WRITE(int socket_cliente, char** parametro1,uint32_t* parametro2, uint32_t* parametro3){
	   size_t size_payload ;
	    if (recv(socket_cliente,&size_payload,sizeof(size_t), 0) != sizeof(size_t)) {
	        return false;
	    }
	    void* stream = malloc(size_payload);

	    if (recv(socket_cliente,stream ,size_payload, 0) != size_payload) {
	        free(stream);
	        return false;
	    }
	    deserializar_F_WRITE(stream, parametro1, parametro2,parametro3);
	    free(stream);
	    return true;
}
bool send_F_WRITE(int socket_cliente, char*  parametro1,uint32_t  parametro2, uint32_t parametro3){
	  size_t size;
	    void* stream = serializar_F_WRITE(&size ,parametro1, parametro2, parametro3);
	    if (send(socket_cliente, stream, size, 0) != size) {
	        free(stream);
	        return false;
	    }
	    free(stream);
	    return true;
}

static void* serializar_F_READ(size_t* size, char* parametro1 ,uint32_t  parametro2 ,uint32_t parametro3){
	size_t size_parametro1 = strlen(parametro1) +1; // No sabemos si agregar +1 PREGUNTAR
	*size =sizeof(op_code) + 2*sizeof(size_t) + sizeof(uint32_t)+ size_parametro1;
	size_t size_payload = *size - sizeof(op_code) - sizeof(size_t);
	void* stream = malloc(*size);
	op_code cop = F_READ;
	    memcpy(stream, &cop, sizeof(op_code));
	    memcpy(stream+sizeof(op_code), &size_payload, sizeof(size_t));
	    memcpy(stream+sizeof(op_code)+sizeof(size_t), &size_parametro1,sizeof(size_t));
	    memcpy(stream+sizeof(op_code)+sizeof(size_t)+sizeof(size_t), parametro1, size_parametro1);
	    memcpy(stream+sizeof(op_code)+sizeof(size_t)+sizeof(size_t) + size_parametro1, &parametro2, sizeof(uint32_t));
	    memcpy(stream+sizeof(op_code)+sizeof(size_t)+sizeof(size_t) + size_parametro1+sizeof(uint32_t), &parametro3, sizeof(uint32_t));
// copio al stream en orden cop,payload,sizep1,p1,p2
	    return stream;
}
static void deserializar_F_READ(void* stream,char** parametro1 , uint32_t* parametro2,uint32_t* parametro3){
	size_t size_parametro1;
	memcpy(&size_parametro1, stream, sizeof(size_t));
	char* p1 = malloc(size_parametro1);
	memcpy(p1,stream+sizeof(size_t) ,size_parametro1);
	*parametro1 = p1;
	memcpy(parametro2,stream+sizeof(size_t)+size_parametro1,sizeof(uint32_t));
	memcpy(parametro3,stream+sizeof(size_t)+size_parametro1+sizeof(uint32_t),sizeof(uint32_t));
}
bool recv_F_READ(int socket_cliente, char** parametro1,uint32_t* parametro2, uint32_t* parametro3){
	  size_t size_payload ;
		    if (recv(socket_cliente,&size_payload,sizeof(size_t), 0) != sizeof(size_t)) {
		        return false;
		    }
		    void* stream = malloc(size_payload);

		    if (recv(socket_cliente,stream ,size_payload, 0) != size_payload) {
		        free(stream);
		        return false;
		    }
		    deserializar_F_READ(stream, parametro1, parametro2,parametro3);
		    free(stream);
		    return true;
}
bool send_F_READ(int socket_cliente, char*  parametro1,uint32_t  parametro2, uint32_t parametro3){
	  size_t size;
	    void* stream = serializar_F_READ(&size ,parametro1, parametro2, parametro3);
	    if (send(socket_cliente, stream, size, 0) != size) {
	        free(stream);
	        return false;
	    }
	    free(stream);
	    return true;
}

static void* serializar_DELETE_SEGMENT(uint32_t parametro1) {
   void* stream = malloc(sizeof(op_code) + sizeof(uint32_t));
   op_code cop = DELETE_SEGMENT;
    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream+sizeof(op_code), &parametro1, sizeof(uint32_t));
    return stream;
}
static void deserializar_DELETE_SEGMENT(void* stream, uint32_t* parametro1) {
    memcpy(parametro1, stream ,sizeof(uint32_t));
}
bool recv_DELETE_SEGMENT(int socket_cliente, uint32_t* parametro1){
    size_t size = sizeof(uint32_t);
    void* stream = malloc(size);
    if (recv(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    deserializar_DELETE_SEGMENT(stream, parametro1);
    free(stream);
    return true;
}
bool send_DELETE_SEGMENT(int socket_cliente, uint32_t  parametro1){
    size_t size = sizeof(op_code) + sizeof(uint32_t);
    void* stream = serializar_DELETE_SEGMENT(parametro1);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

static void* serializar_F_CLOSE(size_t* size,char* archivo){
		size_t size_archivo = strlen(archivo)+1; // No sabemos si agregar +1 PREGUNTAR
		*size =sizeof(op_code) + 2*sizeof(size_t)+ size_archivo;
		size_t size_payload = *size - sizeof(op_code) - sizeof(size_t);
		void* stream = malloc(*size);
		op_code cop = F_CLOSE;
		    memcpy(stream, &cop, sizeof(op_code));
		    memcpy(stream+sizeof(op_code), &size_payload, sizeof(size_t));
		    memcpy(stream+sizeof(op_code)+sizeof(size_t), &size_archivo, sizeof(size_t));
		    memcpy(stream+sizeof(op_code)+sizeof(size_t)+size_archivo, archivo, size_archivo);
		    return stream;
}
static void deserializar_F_CLOSE(void* stream,char** archivo){
	size_t size_archivo;
	memcpy(&size_archivo, stream, sizeof(size_t));
	char* p2 = malloc(size_archivo);
	*archivo = p2;
	memcpy(p2,stream+sizeof(size_t) ,size_archivo);
}
bool recv_F_CLOSE(int socket_cliente, char** archivo){
	 size_t size_payload ;
	    if (recv(socket_cliente,&size_payload,sizeof(size_t), 0) != sizeof(size_t)) {
	        return false;
	    }
	    void* stream = malloc(size_payload);
	    if (recv(socket_cliente,stream ,size_payload, 0) != size_payload) {
	        free(stream);
	        return false;
	    }
	    deserializar_F_CLOSE(stream, archivo);
	    free(stream);
	    return true;
}
bool send_F_CLOSE(int socket_cliente, char* archivo){
    size_t size;
    void* stream = serializar_F_CLOSE(&size ,archivo);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

static void* serializar_EXIT() {
   void* stream = malloc(sizeof(op_code));
    op_code cop = EXIT;
    memcpy(stream, &cop, sizeof(op_code));
    return stream;
}
bool recv_EXIT(int socket_cliente) {
    size_t size = sizeof(op_code);
    void* stream = malloc(size);
    if (recv(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}
bool send_EXIT(int socket_cliente) {
   size_t size = sizeof(op_code);
    void* stream = serializar_EXIT();
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    free(stream);
    return true;
}
static void* serializar_INICIAR_ESTRUCTURA_MEMORIA() {
   void* stream = malloc(sizeof(op_code));
    op_code cop = INICIAR_ESTRUCTURAS;
    memcpy(stream, &cop, sizeof(op_code));
    return stream;
}
bool recv_INICIAR_ESTRUCTURA_MEMORIA(int socket_cliente){

	    size_t size = sizeof(op_code);
	    void* stream = malloc(size);

	    if (recv(socket_cliente, stream, size, 0) != size) {
	        free(stream);
	        return false;
	    }
	    //deserializar_YIELD(stream);
	    free(stream);
	    return true;

}
void send_INICIAR_ESTRUCTURA_MEMORIA(int socket_cliente){
	op_code instruccion = YIELD;
   size_t size = sizeof(op_code);
    void* stream = serializar_INICIAR_ESTRUCTURA_MEMORIA(instruccion);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);

    }
    free(stream);
}

static void* serializar_SEGMENTO(size_t* size, segmento_t* segment) {
    *size = sizeof(uint32_t) * 3;
    void* stream = malloc(*size);
    memcpy(stream, &segment->id, sizeof(uint32_t));
    memcpy(stream + sizeof(uint32_t), &segment->direccion_Base, sizeof(uint32_t));
    memcpy(stream + 2 * sizeof(uint32_t), &segment->tamanio, sizeof(uint32_t));
    return stream;
}
static void deserializar_SEGMENTO(void* stream,segmento_t** segmento){
	segmento_t* segment = malloc(sizeof(segmento_t));
    memcpy(&segment->id, stream, sizeof(uint32_t));
    memcpy(&segment->direccion_Base, stream + sizeof(uint32_t), sizeof(uint32_t));
    memcpy(&segment->tamanio, stream + 2*sizeof(uint32_t), sizeof(uint32_t));
    *segmento = segment;
}
bool send_SEGMENTO(int server_memoria, segmento_t* segmento){
	size_t size;
    void* stream = serializar_SEGMENTO(&size ,segmento);
    if (send(server_memoria, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;

}
bool recv_SEGMENTO(int socket_cliente, segmento_t** segmento){
	    size_t size = sizeof(uint32_t) * 3;
	    void* stream = malloc(size);

	    if (recv(socket_cliente, stream, size, 0) != size) {
	        free(stream);
	        return false;
	    }
	    deserializar_SEGMENTO(stream, segmento);
	    free(stream);
	    return true;
	}
	static void* serializar_PID(uint32_t parametro1) {
   void* stream = malloc(sizeof(uint32_t));
    memcpy(stream, &parametro1, sizeof(uint32_t));
    return stream;
}
static void deserializar_PID(void* stream, uint32_t* parametro1) {
    memcpy(parametro1, stream ,sizeof(uint32_t));

}

bool send_PID(int fd,uint32_t parametro1){
    //printf("Entre en send_PID \n");
       size_t size = sizeof(uint32_t);

        void* stream = serializar_PID(parametro1);


        if (send(fd, stream, size, 0) != size) {
            free(stream);
            return false;
        }

        free(stream);
        return true;
}

bool recv_PID(int fd, uint32_t* parametro1) {
    size_t size = sizeof(uint32_t);
    void* stream = malloc(size);

    if (recv(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    deserializar_PID(stream, parametro1);

    free(stream);
    return true;
}

static void* serializar_PC(uint32_t parametro1) {
   void* stream = malloc(sizeof(uint32_t));
    memcpy(stream, &parametro1, sizeof(uint32_t));
    return stream;
}


static void deserializar_PC(void* stream, uint32_t* parametro1) {
    memcpy(parametro1, stream ,sizeof(uint32_t));

}

bool send_PC(int fd, uint32_t parametro1) {
   size_t size = sizeof(uint32_t);
    void* stream = serializar_PC(parametro1);
    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}


bool recv_PC(int fd, uint32_t* parametro1) {
    size_t size = sizeof(uint32_t);
    void* stream = malloc(size);
    if (recv(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    deserializar_PC(stream, parametro1);
    free(stream);
    return true;
}

static void* serializar_tiempo_bloqueante(uint32_t parametro1) {
   void* stream = malloc(sizeof(uint32_t));
    memcpy(stream, &parametro1, sizeof(uint32_t));
    return stream;
}


static void deserializar_tiempo_bloqueante(void* stream, uint32_t* parametro1) {
    memcpy(parametro1, stream ,sizeof(uint32_t));

}

bool send_tiempo_bloqueante(int fd, uint32_t parametro1) {
   size_t size = sizeof(uint32_t);
    void* stream = serializar_tiempo_bloqueante(parametro1);
    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}


bool recv_tiempo_bloqueante(int fd, uint32_t* parametro1) {
    size_t size = sizeof(uint32_t);
    void* stream = malloc(size);
    if (recv(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    deserializar_tiempo_bloqueante(stream, parametro1);
    free(stream);
    return true;
}

static void* serializar_BASE_SEGMENTO(uint32_t parametro1) {
   void* stream = malloc(sizeof(uint32_t));
    memcpy(stream, &parametro1, sizeof(uint32_t));
    return stream;
}


static void deserializar_BASE_SEGMENTO(void* stream, uint32_t* parametro1) {
    memcpy(parametro1, stream ,sizeof(uint32_t));

}

bool send_BASE_SEGMENTO(int fd, uint32_t parametro1) {
   size_t size = sizeof(uint32_t);
    void* stream = serializar_BASE_SEGMENTO(parametro1);
    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}


bool recv_BASE_SEGMENTO(int fd, uint32_t* parametro1) {
    size_t size = sizeof(uint32_t);
    void* stream = malloc(size);
    if (recv(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    deserializar_BASE_SEGMENTO(stream, parametro1);
    free(stream);
    return true;
}

static void* serializar_ID_SEGMENTO(uint32_t parametro1) {
   void* stream = malloc(sizeof(uint32_t));
    memcpy(stream, &parametro1, sizeof(uint32_t));
    return stream;
}


static void deserializar_ID_SEGMENTO(void* stream, uint32_t* parametro1) {
    memcpy(parametro1, stream ,sizeof(uint32_t));

}

bool send_ID_SEGMENTO(int fd, uint32_t parametro1) {
   size_t size = sizeof(uint32_t);
    void* stream = serializar_ID_SEGMENTO(parametro1);
    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}


bool recv_ID_SEGMENTO(int fd, uint32_t* parametro1) {
    size_t size = sizeof(uint32_t);
    void* stream = malloc(size);
    if (recv(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    deserializar_ID_SEGMENTO(stream, parametro1);
    free(stream);
    return true;
}
static void* serializar_CANT_INSTRUCCIONES(uint32_t parametro1) {
   void* stream = malloc(sizeof(uint32_t));
    memcpy(stream, &parametro1, sizeof(uint32_t));
    return stream;
}


static void deserializar_CANT_INSTRUCCIONES(void* stream, uint32_t* parametro1) {
    memcpy(parametro1, stream ,sizeof(uint32_t));

}

bool send_CANT_INSTRUCCIONES(int fd, uint32_t parametro1) {
   size_t size = sizeof(uint32_t);
    void* stream = serializar_CANT_INSTRUCCIONES(parametro1);
    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}


bool recv_CANT_INSTRUCCIONES(int fd, uint32_t* parametro1) {
    size_t size = sizeof(uint32_t);
    void* stream = malloc(size);
    if (recv(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    deserializar_CANT_INSTRUCCIONES(stream, parametro1);
    free(stream);
    return true;
}


static void* serializar_REG_CPU(registros_cpu reg ){
void* stream = malloc(4*sizeof(char[4]) + 4*sizeof(char[8]) + 4*sizeof(char[16]));
int offset = 0; // Desplazamiento
	memcpy(stream, &reg.AX, sizeof(char[4]));
	offset += sizeof(char[4]);
	memcpy(stream + offset, &reg.BX, sizeof(char[4]));
	offset += sizeof(char[4]);
	memcpy(stream + offset, &reg.CX, sizeof(char[4]));
	offset += sizeof(char[4]);
	memcpy(stream + offset, &reg.DX, sizeof(char[4]));
	offset += sizeof(char[4]);
	memcpy(stream + offset,&reg.EAX,sizeof(char[8]));
	offset += sizeof(char[8]);
	memcpy(stream + offset,&reg.EBX,sizeof(char[8]));
	offset += sizeof(char[8]);
	memcpy(stream + offset,&reg.ECX,sizeof(char[8]));
	offset += sizeof(char[8]);
	memcpy(stream + offset,&reg.EDX,sizeof(char[8]));
	offset += sizeof(char[8]);
	memcpy(stream + offset,&reg.RAX,sizeof(char[16]));
	offset += sizeof(char[16]);
	memcpy(stream + offset,&reg.RBX,sizeof(char[16]));
	offset += sizeof(char[16]);
	memcpy(stream + offset,&reg.RCX,sizeof(char[16]));
	offset += sizeof(char[16]);
	memcpy(stream + offset,&reg.RDX,sizeof(char[16]));

return stream;
}
static void deserializar_REG_CPU(void* stream,registros_cpu* reg ){
int offset = 0; // Desplazamiento
	memcpy(&reg->AX, stream ,sizeof(char[4]));
	offset += sizeof(char[4]);
	memcpy(&reg->BX, stream + offset, sizeof(char[4]));
	offset += sizeof(char[4]);
	memcpy(&reg->CX, stream + offset, sizeof(char[4]));
	offset += sizeof(char[4]);
	memcpy(&reg->DX, stream + offset, sizeof(char[4]));
	offset += sizeof(char[4]);
	memcpy(&reg->EAX, stream + offset,sizeof(char[8]));
	offset += sizeof(char[8]);
	memcpy(&reg->EBX, stream + offset,sizeof(char[8]));
	offset += sizeof(char[8]);
	memcpy(&reg->ECX, stream + offset,sizeof(char[8]));
	offset += sizeof(char[8]);
	memcpy(&reg->EDX, stream + offset,sizeof(char[8]));
	offset += sizeof(char[8]);
	memcpy(&reg->RAX, stream + offset,sizeof(char[16]));
	offset += sizeof(char[16]);
	memcpy(&reg->RBX, stream + offset,sizeof(char[16]));
	offset += sizeof(char[16]);
	memcpy(&reg->RCX, stream + offset,sizeof(char[16]));
	offset += sizeof(char[16]);
	memcpy(&reg->RDX, stream + offset,sizeof(char[16]));
}

bool send_REG_CPU(int fd, registros_cpu registros) {
   size_t size = 4*sizeof(char[4]) + 4*sizeof(char[8]) + 4*sizeof(char[16]);
    void* stream = serializar_REG_CPU(registros);
    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}


bool recv_REG_CPU(int fd, registros_cpu* registros) {
    size_t size = 4*sizeof(char[4]) + 4*sizeof(char[8]) + 4*sizeof(char[16]);
    void* stream = malloc(size);
    if (recv(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    deserializar_REG_CPU(stream, registros);
    free(stream);
    return true;
}

static void* serializar_CONTEXTO_EJECUCION(contexto_ejecucion contexto){
	    void* stream = malloc(2 * sizeof(uint32_t) + 4 * sizeof(char[4]) + 4 * sizeof(char[8]) + 4 * sizeof(char[16]));
	    //+sizeof(t_list);// TODO este sizeof seria el de la tabla de segmentos

	    // Serializar los datos individualmente y copiarlos en el stream
	    int offset = 0;

	    memcpy(stream + offset, &contexto.PID, sizeof(uint32_t));
	    offset += sizeof(uint32_t);

	    memcpy(stream + offset, &contexto.PC, sizeof(uint32_t));
	    offset += sizeof(uint32_t);

	    memcpy(stream + offset, contexto.registros.AX, sizeof(char[4]));
	    offset += sizeof(char[4]);

	    memcpy(stream + offset, contexto.registros.BX, sizeof(char[4]));
	    offset += sizeof(char[4]);

	    memcpy(stream + offset, contexto.registros.CX, sizeof(char[4]));
	    offset += sizeof(char[4]);

	    memcpy(stream + offset, contexto.registros.DX, sizeof(char[4]));
	    offset += sizeof(char[4]);

	    memcpy(stream + offset, contexto.registros.EAX, sizeof(char[8]));
	    offset += sizeof(char[8]);

	    memcpy(stream + offset, contexto.registros.EBX, sizeof(char[8]));
	    offset += sizeof(char[8]);

	    memcpy(stream + offset, contexto.registros.ECX, sizeof(char[8]));
	    offset += sizeof(char[8]);

	    memcpy(stream + offset, contexto.registros.EDX, sizeof(char[8]));
	    offset += sizeof(char[8]);

	    memcpy(stream + offset, contexto.registros.RAX, sizeof(char[16]));
	    offset += sizeof(char[16]);

	    memcpy(stream + offset, contexto.registros.RBX, sizeof(char[16]));
	    offset += sizeof(char[16]);

	    memcpy(stream + offset, contexto.registros.RCX, sizeof(char[16]));
	    offset += sizeof(char[16]);

	    memcpy(stream + offset, contexto.registros.RDX, sizeof(char[16]));

	// lista de la tabla de segmentos tambien iria aca TODO
	return stream;
}
static void deserializar_CONTEXTO_EJECUCION(void* stream, contexto_ejecucion*contexto){
	    int offset = 0; //todo asegurarse si va  u_int32

	    memcpy(&contexto->PID, stream + offset, sizeof(uint32_t));
	    offset += sizeof(uint32_t);

	    memcpy(&contexto->PC, stream + offset, sizeof(uint32_t));
	    offset += sizeof(uint32_t);

	    memcpy(contexto->registros.AX, stream + offset, sizeof(char[4]));
	    offset += sizeof(char[4]);

	    memcpy(contexto->registros.BX, stream + offset, sizeof(char[4]));
	    offset += sizeof(char[4]);

	    memcpy(contexto->registros.CX, stream + offset, sizeof(char[4]));
	    offset += sizeof(char[4]);

	    memcpy(contexto->registros.DX, stream + offset, sizeof(char[4]));
	    offset += sizeof(char[4]);

	    memcpy(contexto->registros.EAX, stream + offset, sizeof(char[8]));
	    offset += sizeof(char[8]);

	    memcpy(contexto->registros.EBX, stream + offset, sizeof(char[8]));
	    offset += sizeof(char[8]);

	    memcpy(contexto->registros.ECX, stream + offset, sizeof(char[8]));
	    offset += sizeof(char[8]);

	    memcpy(contexto->registros.EDX, stream + offset, sizeof(char[8]));
	    offset += sizeof(char[8]);

	    memcpy(contexto->registros.RAX, stream + offset, sizeof(char[16]));
	    offset += sizeof(char[16]);

	    memcpy(contexto->registros.RBX, stream + offset, sizeof(char[16]));
	    offset += sizeof(char[16]);

	    memcpy(contexto->registros.RCX, stream + offset, sizeof(char[16]));
	    offset += sizeof(char[16]);

	    memcpy(contexto->registros.RDX, stream + offset, sizeof(char[16]));
	    //lista de tabla de segmentos //todo
}
bool send_CONTEXTO_EJECUCION(int fd, contexto_ejecucion contexto){
	size_t size = 2*sizeof(uint32_t)
	+4*sizeof(char[4]) + 4*sizeof(char[8]) + 4*sizeof(char[16]);
	//+sizeof(t_list);// TODO este sizeof seria el de la tabla de segmentos

    void* stream = serializar_CONTEXTO_EJECUCION(contexto);

    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}
bool recv_CONTEXTO_EJECUCION(int fd, contexto_ejecucion* contexto){
    size_t size = 2*sizeof(uint32_t) +4*sizeof(char[4]) + 4*sizeof(char[8]) + 4*sizeof(char[16]);// mas lista todo
    void* stream = malloc(size);
    if (recv(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    deserializar_CONTEXTO_EJECUCION(stream, contexto);
    free(stream);
    return true;
}

static void* serializar_handshake(uint8_t resultado){
	void*stream = malloc(sizeof(op_code) + sizeof(uint8_t));
	op_code cop = HANDSHAKE;
	memcpy(stream,&cop,sizeof(op_code));
	memcpy(stream+sizeof(op_code),&resultado,sizeof(uint8_t));
    return stream;
}


static void deserializar_handshake(void*stream,uint8_t*resultado){
	memcpy(resultado,stream,sizeof(uint8_t));
}

bool send_handshake(int socket,uint8_t resultado){
	size_t size = sizeof(op_code) + sizeof(uint8_t);
	void*stream = serializar_handshake(resultado);
	if(send(socket,stream,size,0) != size){
		free(stream);
		return false;
	}
	free(stream);
	return true;
}
bool recv_handshake(int socket,uint8_t* resultado){
	size_t size = sizeof(uint8_t);
	void*stream = malloc(size);
	if(recv(socket,stream,size,0)!=size){
		free(stream);
		return false;
	}
	deserializar_handshake(stream,resultado);
	free(stream);
	return (true);
}

static void* serializar_TABLA_SEGMENTOS(size_t* size, t_list* lista) {
    *size = 3* sizeof(uint32_t)  * list_size(lista); // sizeof(uint32_t) por cada campo (ID, base, tamaño)
    void*stream = malloc(*size);

    // Serializo los elementos
    t_list_iterator* list_it = list_iterator_create(lista);
    int i = 0;
    while (list_iterator_has_next(list_it)) {
        segmento_t* segmento = list_iterator_next(list_it);
        memcpy(stream + i * sizeof(uint32_t), &(segmento->id), sizeof(uint32_t));
        memcpy(stream + (i + 1) * sizeof(uint32_t), &(segmento->direccion_Base), sizeof(uint32_t));
        memcpy(stream + (i + 2) * sizeof(uint32_t), &(segmento->tamanio), sizeof(uint32_t));
                i += 3;
    }
    list_iterator_destroy(list_it);
    return stream;
}



static t_list* deserializar_TABLA_SEGMENTOS(void* stream, uint32_t n_elements) {
    t_list* lista = list_create();

    // Deserializo y los agrego a la lista
    for (uint32_t i = 0; n_elements > 0; i += 3, n_elements--) {
    	segmento_t* segmento = malloc(sizeof(segmento_t));
        memcpy(&(segmento->id), stream + i * sizeof(uint32_t), sizeof(uint32_t));
        memcpy(&(segmento->direccion_Base), stream + (i + 1) * sizeof(uint32_t), sizeof(uint32_t));
        memcpy(&(segmento->tamanio), stream + (i + 2) * sizeof(uint32_t), sizeof(uint32_t));
        list_add(lista, segmento);
    }
    return lista;
}

bool send_TABLA_SEGMENTOS(int fd, t_list* lista) {
    size_t size;
    void* stream = serializar_TABLA_SEGMENTOS(&size, lista);
    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

bool recv_TABLA_SEGMENTOS(int fd, t_list** lista) {
    size_t size;
    void* stream = malloc(size);
    if (recv(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    uint8_t n_elements = size / (sizeof(uint32_t) * 3); // Cálculo del número de elementos en la lista
    *lista = deserializar_TABLA_SEGMENTOS(stream, n_elements);
    free(stream);
    return true;
}


























