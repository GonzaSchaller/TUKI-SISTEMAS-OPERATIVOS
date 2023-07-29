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
static void* serializar_EXISTE_ARCHIVO(size_t* size,char* recurso){
		size_t size_recurso = strlen(recurso)+1;
		*size =sizeof(op_code) + 2*sizeof(size_t)+ size_recurso;
		size_t size_payload = *size - sizeof(op_code) - sizeof(size_t);
		void* stream = malloc(*size);
		op_code cop = EXISTE_ARCHIVO;
		    memcpy(stream, &cop, sizeof(op_code));
		    memcpy(stream+sizeof(op_code), &size_payload, sizeof(size_t));
		    memcpy(stream+sizeof(op_code)+sizeof(size_t), &size_recurso, sizeof(size_t));
		    memcpy(stream+sizeof(op_code)+sizeof(size_t)+sizeof(size_t), recurso, size_recurso);
	// copio al stream en orden cop,payload,sizep2,p2
		    return stream;

}
static void deserializar_EXISTE_ARCHIVO(void* stream,char** recurso){
	size_t size_archivo;
	memcpy(&size_archivo, stream, sizeof(size_t));
	char* p2 = malloc(size_archivo);
	*recurso = p2;
	memcpy(p2,stream+sizeof(size_t) ,size_archivo);
// OPCODE,PAYLOAD, SIZE P2, P2
}
bool recv_EXISTE_ARCHIVO(int socket_cliente, char** recurso){
	 size_t size_payload ;
	    if (recv(socket_cliente,&size_payload,sizeof(size_t), 0) != sizeof(size_t)) {
	        return false;
	    }
	    void* stream = malloc(size_payload);

	    if (recv(socket_cliente,stream ,size_payload, 0) != size_payload) {
	        free(stream);
	        return false;
	    }
	    deserializar_EXISTE_ARCHIVO(stream, recurso);
	    free(stream);
	    return true;
}
bool send_EXISTE_ARCHIVO(int socket_cliente, char* recurso){
    size_t size;
    void* stream = serializar_EXISTE_ARCHIVO(&size ,recurso);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

static void* serializar_F_OPEN(size_t* size,char* recurso){
		size_t size_recurso = strlen(recurso)+1;
		*size =sizeof(op_code) + 2*sizeof(size_t)+ size_recurso;
		size_t size_payload = *size - sizeof(op_code) - sizeof(size_t);
		void* stream = malloc(*size);
		op_code cop = F_OPEN;
		    memcpy(stream, &cop, sizeof(op_code));
		    memcpy(stream+sizeof(op_code), &size_payload, sizeof(size_t));
		    memcpy(stream+sizeof(op_code)+sizeof(size_t), &size_recurso, sizeof(size_t));
		    memcpy(stream+sizeof(op_code)+sizeof(size_t)+sizeof(size_t), recurso, size_recurso);
	// copio al stream en orden cop,payload,sizep2,p2
		    return stream;

}
static void deserializar_F_OPEN(void* stream,char** recurso){
	size_t size_archivo;
	memcpy(&size_archivo, stream, sizeof(size_t));
	char* p2 = malloc(size_archivo);
	*recurso = p2;
	memcpy(p2,stream+sizeof(size_t) ,size_archivo);
// OPCODE,PAYLOAD, SIZE P2, P2
}
bool recv_F_OPEN(int socket_cliente, char** recurso){
	 size_t size_payload ;
	    if (recv(socket_cliente,&size_payload,sizeof(size_t), 0) != sizeof(size_t)) {
	        return false;
	    }
	    void* stream = malloc(size_payload);

	    if (recv(socket_cliente,stream ,size_payload, 0) != size_payload) {
	        free(stream);
	        return false;
	    }
	    deserializar_F_OPEN(stream, recurso);
	    free(stream);
	    return true;
}
bool send_F_OPEN(int socket_cliente, char* recurso){
    size_t size;
    void* stream = serializar_F_OPEN(&size ,recurso);
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
   size_t size = sizeof(op_code);
    void* stream = serializar_YIELD();
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

static void* serializar_F_SEEK(size_t* size, char* parametro1 ,uint32_t  parametro2 ){
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
static void deserializar_F_SEEK(void* stream,char** parametro1 , uint32_t* parametro2){
	size_t size_parametro1;
	memcpy(&size_parametro1, stream, sizeof(size_t));
	char* p1 = malloc(size_parametro1);
	memcpy(p1,stream+sizeof(size_t) ,size_parametro1);
	*parametro1 = p1;
	memcpy(parametro2,stream+sizeof(size_t)+size_parametro1,sizeof(uint32_t));
// OPCODE,PAYLOAD, SIZE P2, P2,ENTERO P1,
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

static void* serializar_F_WRITE(size_t* size, char* parametro1, uint32_t parametro2, uint32_t parametro3) {
    size_t size_parametro1 = strlen(parametro1) + 1;
    *size = sizeof(op_code) + 2 * sizeof(size_t) + size_parametro1 + sizeof(uint32_t) + sizeof(uint32_t);
    size_t size_payload = *size - sizeof(op_code) - sizeof(size_t);
    void* stream = malloc(*size);
    op_code cop = F_WRITE;
    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream + sizeof(op_code), &size_payload, sizeof(size_t));
    memcpy(stream + sizeof(op_code) + sizeof(size_t), &size_parametro1, sizeof(size_t));
    memcpy(stream + sizeof(op_code) + 2 * sizeof(size_t), parametro1, size_parametro1);
    memcpy(stream + sizeof(op_code) + 2 * sizeof(size_t) + size_parametro1, &parametro2, sizeof(uint32_t));
    memcpy(stream + sizeof(op_code) + 2 * sizeof(size_t) + size_parametro1 + sizeof(uint32_t), &parametro3, sizeof(uint32_t));
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

static void* serializar_F_READ(size_t* size, char* parametro1, uint32_t parametro2, uint32_t parametro3) {
    size_t size_parametro1 = strlen(parametro1) + 1;
    *size = sizeof(op_code) + 2 * sizeof(size_t) + size_parametro1 + sizeof(uint32_t) + sizeof(uint32_t);
    size_t size_payload = *size - sizeof(op_code) - sizeof(size_t);
    void* stream = malloc(*size);
    op_code cop = F_READ;
    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream + sizeof(op_code), &size_payload, sizeof(size_t));
    memcpy(stream + sizeof(op_code) + sizeof(size_t), &size_parametro1, sizeof(size_t));
    memcpy(stream + sizeof(op_code) + 2 * sizeof(size_t), parametro1, size_parametro1);
    memcpy(stream + sizeof(op_code) + 2 * sizeof(size_t) + size_parametro1, &parametro2, sizeof(uint32_t));
    memcpy(stream + sizeof(op_code) + 2 * sizeof(size_t) + size_parametro1 + sizeof(uint32_t), &parametro3, sizeof(uint32_t));
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

static void* serializar_F_CLOSE(size_t* size,char* recurso){
		size_t size_recurso = strlen(recurso)+1;
		*size =sizeof(op_code) + 2*sizeof(size_t)+ size_recurso;
		size_t size_payload = *size - sizeof(op_code) - sizeof(size_t);
		void* stream = malloc(*size);
		op_code cop = F_CLOSE;
		    memcpy(stream, &cop, sizeof(op_code));
		    memcpy(stream+sizeof(op_code), &size_payload, sizeof(size_t));
		    memcpy(stream+sizeof(op_code)+sizeof(size_t), &size_recurso, sizeof(size_t));
		    memcpy(stream+sizeof(op_code)+sizeof(size_t)+sizeof(size_t), recurso, size_recurso);
	// copio al stream en orden cop,payload,sizep2,p2
		    return stream;

}
static void deserializar_F_CLOSE(void* stream,char** recurso){
	size_t size_archivo;
	memcpy(&size_archivo, stream, sizeof(size_t));
	char* p2 = malloc(size_archivo);
	*recurso = p2;
	memcpy(p2,stream+sizeof(size_t) ,size_archivo);
// OPCODE,PAYLOAD, SIZE P2, P2
}
bool recv_F_CLOSE(int socket_cliente, char** recurso){
	 size_t size_payload ;
	    if (recv(socket_cliente,&size_payload,sizeof(size_t), 0) != sizeof(size_t)) {
	        return false;
	    }
	    void* stream = malloc(size_payload);

	    if (recv(socket_cliente,stream ,size_payload, 0) != size_payload) {
	        free(stream);
	        return false;
	    }
	    deserializar_F_CLOSE(stream, recurso);
	    free(stream);
	    return true;
}
bool send_F_CLOSE(int socket_cliente, char* recurso){
    size_t size;
    void* stream = serializar_F_CLOSE(&size ,recurso);
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
	op_code instruccion = INICIAR_ESTRUCTURAS;
   size_t size = sizeof(op_code);
    void* stream = serializar_INICIAR_ESTRUCTURA_MEMORIA(instruccion);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return;
    }
    free(stream);
}

static void* serializar_SEGMENTO(size_t* size, segmento_t* segment) {
    *size = sizeof(segmento_t);
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
	    size_t size = sizeof(segmento_t);
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
    void* stream = malloc( sizeof(uint32_t));
    memcpy(stream, &parametro1, sizeof(uint32_t));
    return stream;
}
static void deserializar_PID(void* stream, uint32_t* parametro1) {
    memcpy(parametro1, stream, sizeof(uint32_t));
}
bool recv_PID(int socket_cliente, uint32_t* parametro1){
    size_t size = sizeof(uint32_t);
    void* stream = malloc(size);
    if (recv(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    deserializar_PID(stream, parametro1);
    free(stream);
	return true;
}
bool send_PID(int socket_cliente, uint32_t  parametro1){
    size_t size = sizeof(uint32_t);
    void* stream = serializar_PID(parametro1);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

static void* serializar_PC(uint32_t parametro1) {
    void* stream = malloc( sizeof(uint32_t));
    memcpy(stream, &parametro1, sizeof(uint32_t));
    return stream;
}
static void deserializar_PC(void* stream, uint32_t* parametro1) {
    memcpy(parametro1, stream, sizeof(uint32_t));
}
bool recv_PC(int socket_cliente, uint32_t* parametro1){
    size_t size = sizeof(uint32_t);
    void* stream = malloc(size);
    if (recv(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    deserializar_PC(stream, parametro1);
    free(stream);
	return true;
}
bool send_PC(int socket_cliente, uint32_t  parametro1){
    size_t size = sizeof(uint32_t);
    void* stream = serializar_PC(parametro1);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

static void* serializar_tiempo_bloqueante(uint32_t parametro1) {
   void* stream = malloc(sizeof(float));
    memcpy(stream, &parametro1, sizeof(uint32_t));
    return stream;
}
static void deserializar_tiempo_bloqueante(void* stream, float* parametro1) {
    memcpy(parametro1, stream ,sizeof(float));

}
bool send_tiempo_bloqueante(int fd, float parametro1) {
   size_t size = sizeof(float);
    void* stream = serializar_tiempo_bloqueante(parametro1);
    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}
bool recv_tiempo_bloqueante(int fd, float* parametro1) {
    size_t size = sizeof(float);
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
    void* stream = malloc( sizeof(uint32_t));
    memcpy(stream, &parametro1, sizeof(uint32_t));
    return stream;
}
static void deserializar_CANT_INSTRUCCIONES(void* stream, uint32_t* parametro1) {
    memcpy(parametro1, stream, sizeof(uint32_t));
}
bool recv_CANT_INSTRUCCIONES(int socket_cliente, uint32_t* parametro1){
    size_t size = sizeof(uint32_t);
    void* stream = malloc(size);
    if (recv(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    deserializar_CANT_INSTRUCCIONES(stream, parametro1);
    free(stream);
	return true;
}
bool send_CANT_INSTRUCCIONES(int socket_cliente, uint32_t  parametro1){
    size_t size = sizeof(uint32_t);
    void* stream = serializar_CANT_INSTRUCCIONES(parametro1);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}
static void* serializar_REG_CPU(registros_cpu reg ){
void* stream = malloc(sizeof(registros_cpu));
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
	memcpy(reg->AX, stream ,sizeof(char[4]));
	offset += sizeof(char[4]);
	memcpy(reg->BX, stream + offset, sizeof(char[4]));
	offset += sizeof(char[4]);
	memcpy(reg->CX, stream + offset, sizeof(char[4]));
	offset += sizeof(char[4]);
	memcpy(reg->DX, stream + offset, sizeof(char[4]));
	offset += sizeof(char[4]);
	memcpy(reg->EAX, stream + offset,sizeof(char[8]));
	offset += sizeof(char[8]);
	memcpy(reg->EBX, stream + offset,sizeof(char[8]));
	offset += sizeof(char[8]);
	memcpy(reg->ECX, stream + offset,sizeof(char[8]));
	offset += sizeof(char[8]);
	memcpy(reg->EDX, stream + offset,sizeof(char[8]));
	offset += sizeof(char[8]);
	memcpy(reg->RAX, stream + offset,sizeof(char[16]));
	offset += sizeof(char[16]);
	memcpy(reg->RBX, stream + offset,sizeof(char[16]));
	offset += sizeof(char[16]);
	memcpy(reg->RCX, stream + offset,sizeof(char[16]));
	offset += sizeof(char[16]);
	memcpy(reg->RDX, stream + offset,sizeof(char[16]));
}
bool send_REG_CPU(int fd, registros_cpu registros) {
   size_t size = sizeof(registros_cpu);
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


static void* serializar_lista(t_list* lista) {
    if (list_is_empty(lista)) {
        return NULL;
    }
    uint32_t num_elementos = lista->elements_count;
    size_t size = sizeof(segmento_t) * num_elementos;
    void* stream = malloc(size);
    t_link_element* current = lista->head;
    for (uint32_t i = 0; i < num_elementos; i++) {
        memcpy(stream + sizeof(segmento_t) * i, current->data, sizeof(segmento_t));
        current = current->next;
    }
    return stream;
}

static void deserializar_lista(void* stream, t_list* lista, uint32_t tamanio_serializado) {
    list_clean(lista);

    int num_elementos = tamanio_serializado/sizeof(segmento_t);

    for (uint32_t i = 0; i < num_elementos; i++) {
    	segmento_t* elemento = malloc(sizeof(segmento_t));
        memcpy(elemento, stream + sizeof(segmento_t) * i, sizeof(segmento_t));
        list_add(lista, elemento);
    }
}

// Funciones de serialización y deserialización de contexto_ejecucion


//void send_CONTEXTO_EJECUCION(int fd,contexto_ejecucion contexto){
//	send_PID(fd, contexto.PC);
//	send_PC(fd, contexto.PC);
//	send_REG_CPU(fd,contexto.registros);
//	send_TABLA_SEGMENTOS(fd,contexto.TSegmento);
//
//}
//
//void recv_CONTEXTO_EJECUCION(int fd,contexto_ejecucion* contexto){
//	recv_PID(fd, &(contexto->PID));
//	recv_PC(fd, &(contexto->PC));
//	recv_REG_CPU(fd, &(contexto->registros));
//	recv_TABLA_SEGMENTOS(fd,&(contexto->TSegmento));
//
//}
static void* serializar_handshake(uint32_t parametro1) {
   void* stream = malloc(sizeof(op_code) + sizeof(uint32_t));
   op_code cop = HANDSHAKE;
    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream+sizeof(op_code), &parametro1, sizeof(uint32_t));
    return stream;
}
static void deserializar_handshake(void* stream, uint32_t* parametro1) {
    memcpy(parametro1, stream ,sizeof(uint32_t));
}
bool recv_handshake(int socket_cliente, uint32_t* parametro1){
    size_t size = sizeof(uint32_t);
    void* stream = malloc(size);
    if (recv(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    deserializar_handshake(stream, parametro1);
    free(stream);
    return true;
}
bool send_handshake(int socket_cliente, uint32_t  parametro1){
    size_t size = sizeof(op_code) + sizeof(uint32_t);
    void* stream = serializar_handshake(parametro1);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}


static void* serializar_TABLA_SEGMENTOS(t_list* tabla_segmentos, int* tamanio_serializado) {
    *tamanio_serializado = 0;
    void* stream = serializar_lista(tabla_segmentos);
    if (stream != NULL) {
        *tamanio_serializado = sizeof(uint32_t) + tabla_segmentos->elements_count * sizeof(segmento_t);
    }
    return stream;
}

static t_list* deserializar_TABLA_SEGMENTOS(void* stream, int tamanio_serializado) {
    t_list* tabla_segmentos = list_create();
    deserializar_lista(stream, tabla_segmentos, tamanio_serializado);
    return tabla_segmentos;
}

bool send_TABLA_SEGMENTOS(int fd, t_list* lista_segmentos) {
    int tamanio_serializado;
    void* stream = serializar_TABLA_SEGMENTOS(lista_segmentos, &tamanio_serializado);

    if (tamanio_serializado == 0) {
        return false;  // La lista está vacía, no se envía nada
    }

    if (send(fd, &tamanio_serializado, sizeof(int), 0) != sizeof(int)) {
        free(stream);
        return false;
    }

    if (send(fd, stream, tamanio_serializado, 0) != tamanio_serializado) {
        free(stream);
        return false;
    }

    free(stream);
    return true;
}

bool recv_TABLA_SEGMENTOS(int fd, t_list** lista_segmentos) {
    int tamanio_serializado;
    if (recv(fd, &tamanio_serializado, sizeof(int), 0) != sizeof(int)) {
        return false;
    }

    void* stream = malloc(tamanio_serializado);
    if (recv(fd, stream, tamanio_serializado, 0) != tamanio_serializado) {
        free(stream);
        return false;
    }

    *lista_segmentos = deserializar_TABLA_SEGMENTOS(stream, tamanio_serializado);

    free(stream);
    return true;
}
static void* serializar_FINALIZAR_ESTRUCTURAS() {
   void* stream = malloc(sizeof(op_code));
    op_code cop = FINALIZAR_ESTRUCTURAS;
    memcpy(stream, &cop, sizeof(op_code));
    return stream;
}
bool recv_FINALIZAR_ESTRUCTURAS(int socket_cliente){

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
bool send_FINALIZAR_ESTRUCTURAS(int socket_cliente){
	op_code instruccion = FINALIZAR_ESTRUCTURAS;
   size_t size = sizeof(op_code);
    void* stream = serializar_FINALIZAR_ESTRUCTURAS(instruccion);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    free(stream);
    return true;
}
//sin cod op
static void* serializar_EXISTE_ARCHIVO2(size_t* size,char* archivo){
		size_t size_archivo = strlen(archivo)+1; // No sabemos si agregar +1 PREGUNTAR
		*size =2*sizeof(size_t)+ size_archivo;
		size_t size_payload = *size- sizeof(size_t);
		void* stream = malloc(*size);
		    memcpy(stream, &size_payload, sizeof(size_t));
		    memcpy(stream+sizeof(size_t), &size_archivo, sizeof(size_t));
		    memcpy(stream+sizeof(size_t)+size_archivo, archivo, size_archivo);
	// copio al stream en orden cop,payload,sizep2,p2
		    return stream;
}

static void deserializar_EXISTE_ARCHIVO2(void* stream,char** archivo){
	size_t size_archivo;
	memcpy(&size_archivo, stream, sizeof(size_t));
	char* p2 = malloc(size_archivo);
	*archivo = p2;
	memcpy(p2,stream+sizeof(size_t) ,size_archivo);
// OPCODE,PAYLOAD, SIZE P2, P2
}
bool recv_EXISTE_ARCHIVO2(int socket_cliente, char** archivo){
	 size_t size_payload ;

	    if (recv(socket_cliente,&size_payload,sizeof(size_t), 0) != sizeof(size_t)) {
	        return false;
	    }
	    void* stream = malloc(size_payload);

	    if (recv(socket_cliente,stream ,size_payload, 0) != size_payload) {
	        free(stream);
	        return false;
	    }
	    deserializar_EXISTE_ARCHIVO2(stream, archivo);
	    free(stream);
	    return true;
}
bool send_EXISTE_ARCHIVO2(int socket_cliente, char* archivo){
    size_t size;
    void* stream = serializar_EXISTE_ARCHIVO2(&size ,archivo);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}
static void* serializar_OK_CODE(extra_code parametro1) {
   void* stream = malloc(sizeof(uint32_t));
   extra_code cop = parametro1;
   memcpy(stream, &cop, sizeof(extra_code));

    return stream;
}
static void deserializar_OK_CODE(void* stream, extra_code* parametro1) {
    memcpy(parametro1, stream ,sizeof(uint32_t));
}
bool recv_OK_CODE(int socket_cliente, extra_code* parametro1){
    size_t size = sizeof(uint32_t);
    void* stream = malloc(size);
    if (recv(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    deserializar_OK_CODE(stream, parametro1);
    free(stream);
    return true;
}
bool send_OK_CODE(int socket_cliente, extra_code  parametro1){
    size_t size = sizeof(extra_code);
    void* stream = serializar_OK_CODE(parametro1);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

static void* serializar_CREAR_ARCHIVO(size_t* size, char* parametro1, uint32_t parametro2) {
    size_t size_parametro1 = strlen(parametro1) + 1;
    *size = 2 * sizeof(size_t) + sizeof(uint32_t) + size_parametro1;
    size_t size_payload = *size - sizeof(size_t);
    void* stream = malloc(*size);
    memcpy(stream, &size_payload, sizeof(size_t));
    memcpy(stream + sizeof(size_t), &size_parametro1, sizeof(size_t));
    memcpy(stream + 2 * sizeof(size_t), parametro1, size_parametro1);
    memcpy(stream + 2 * sizeof(size_t) + size_parametro1, &parametro2, sizeof(uint32_t));
    // copio al stream en orden payload, size_parametro1, parametro1, parametro2
    return stream;
}

static void deserializar_CREAR_ARCHIVO(void* stream, char** parametro1, uint32_t* parametro2) {
    size_t size_parametro1;
    memcpy(&size_parametro1, stream, sizeof(size_t));
    char* p1 = malloc(size_parametro1);
    memcpy(p1, stream + sizeof(size_t), size_parametro1);
    *parametro1 = p1;
    memcpy(parametro2, stream + sizeof(size_t) + size_parametro1, sizeof(uint32_t));
    // PAYLOAD, SIZE P2, P2, ENTERO P1
}
bool recv_CREAR_ARCHIVO(int socket_cliente, char** parametro1,uint32_t* parametro2){
    size_t size_payload ;
    if (recv(socket_cliente,&size_payload,sizeof(size_t), 0) != sizeof(size_t)) {
        return false;
    }
    void* stream = malloc(size_payload);

    if (recv(socket_cliente,stream ,size_payload, 0) != size_payload) {
        free(stream);
        return false;
    }
    deserializar_CREAR_ARCHIVO(stream, parametro1, parametro2);
    free(stream);
    return true;
}
bool send_CREAR_ARCHIVO(int socket_cliente, char*  parametro1,uint32_t  parametro2){
    size_t size;
    void* stream = serializar_CREAR_ARCHIVO(&size ,parametro1, parametro2);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

static void* serializar_FINALIZAR_TRUNCATE() {
   void* stream = malloc(sizeof(extra_code));
    extra_code cop = FINALIZAR;
    memcpy(stream, &cop, sizeof(extra_code));
    return stream;
}
bool recv_FINALIZAR_TRUNCATE(int socket_cliente,uint32_t* instruccion){
	    size_t size = sizeof(extra_code);
	    void* stream = malloc(size);
	    if (recv(socket_cliente, stream, size, 0) != size) {
	        free(stream);
	        return false;
	    }
	    memcpy(instruccion, stream ,sizeof(uint32_t));

	    free(stream);
	    return true;
}
bool send_FINALIZAR_TRUNCATE(int socket_cliente, uint32_t){
   size_t size = sizeof(extra_code);
    void* stream = serializar_FINALIZAR_TRUNCATE();
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    free(stream);
    return true;
}

static void* serializar_FINALIZAR_READ() {
   void* stream = malloc(sizeof(extra_code));
    extra_code cop = FINALIZAR;
    memcpy(stream, &cop, sizeof(extra_code));
    return stream;
}
bool recv_FINALIZAR_READ(int socket_cliente, uint32_t* instruccion){
	    size_t size = sizeof(extra_code);
	    void* stream = malloc(size);
	    if (recv(socket_cliente, stream, size, 0) != size) {
	        free(stream);
	        return false;
	    }
	    memcpy(instruccion, stream ,sizeof(uint32_t));

	    free(stream);
	    return true;
}
bool send_FINALIZAR_READ(int socket_cliente, uint32_t){
   size_t size = sizeof(extra_code);
    void* stream = serializar_FINALIZAR_READ();
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    free(stream);
    return true;
}
static void* serializar_FINALIZAR_WRITE() {
   void* stream = malloc(sizeof(extra_code));
    extra_code cop = FINALIZAR;
    memcpy(stream, &cop, sizeof(extra_code));
    return stream;
}
bool recv_FINALIZAR_WRITE(int socket_cliente,uint32_t* instruccion){
	    size_t size = sizeof(extra_code);
	    void* stream = malloc(size);
	    if (recv(socket_cliente, stream, size, 0) != size) {
	        free(stream);
	        return false;
	    }
	    memcpy(instruccion, stream ,sizeof(uint32_t));

	    free(stream);
	    return true;
}
bool send_FINALIZAR_WRITE(int socket_cliente, uint32_t){
   size_t size = sizeof(extra_code);
    void* stream = serializar_FINALIZAR_WRITE();
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    free(stream);
    return true;
}


//si un profe de disenio ve esto se pega un tiro//

//static void* serializar_READ2(size_t* size, char* parametro2, uint32_t parametro1) {
	// Carga los parametros al reves pero funciona como deberia funcionar
//	size_t size_parametro2 = strlen(parametro2) + 1;
//    *size =
//          sizeof(op_code)   // cop
//        + sizeof(size_t)    // total
  //      + sizeof(size_t)    // size de char* parametro2
      //  + size_parametro2         // char* parametro2
    //    + sizeof(uint32_t);  // parametro1
  //  size_t size_payload = *size - sizeof(op_code) - sizeof(size_t);
   // void* stream = malloc(*size);
   // op_code cop = READ;
   // memcpy(stream, &cop, sizeof(op_code));
    //memcpy(stream+sizeof(op_code), &size_payload, sizeof(size_t));
   // memcpy(stream+sizeof(op_code)+sizeof(size_t), &size_parametro2, sizeof(size_t));
    //memcpy(stream+sizeof(op_code)+sizeof(size_t)*2, parametro2, size_parametro2);
    //memcpy(stream+sizeof(op_code)+sizeof(size_t)*2+size_parametro2, &parametro1, sizeof(uint32_t));

  //  return stream;
//}
//
	// Carga los parametros al reves pero funciona como deberia funcionar
	//size_t size_parametro2;
    //memcpy(&size_parametro2, stream, sizeof(size_t));

//    char* p2 = malloc(size_parametro2);
 //   memcpy(p2, stream+sizeof(size_t), size_parametro2);
  //  *parametro2 = p2;

//    memcpy(parametro1, stream+sizeof(size_t)+size_parametro2, sizeof(uint32_t));
//}
/*bool recv_READ2(int fd,uint32_t* parametro1,char** parametro2) {

    size_t size_payload;
    if (recv(fd, &size_payload, sizeof(size_t), 0) != sizeof(size_t))
        return false;

    void* stream = malloc(size_payload);
    if (recv(fd, stream, size_payload, 0) != size_payload) {
        free(stream);
        return false;
    }

    deserializar_READ2(stream, parametro2, parametro1);

    free(stream);
    return true;
}
bool send_READ2(int fd,uint32_t parametro1 , char* parametro2) {
    size_t size;
    void* stream = serializar_READ2(&size, parametro2, parametro1);
    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

*/
static void* serializar_WRITE_CPU(size_t* size, char* parametro2, uint32_t parametro1) {
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
    op_code cop = WRITE_CPU;
    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream+sizeof(op_code), &size_payload, sizeof(size_t));
    memcpy(stream+sizeof(op_code)+sizeof(size_t), &size_parametro2, sizeof(size_t));
    memcpy(stream+sizeof(op_code)+sizeof(size_t)*2, parametro2, size_parametro2);
    memcpy(stream+sizeof(op_code)+sizeof(size_t)*2+size_parametro2, &parametro1, sizeof(uint32_t));

    return stream;
}
static void deserializar_WRITE_CPU(void* stream, char** parametro2, uint32_t* parametro1) {
	// Carga los parametros al reves pero funciona como deberia funcionar
	size_t size_parametro2;
    memcpy(&size_parametro2, stream, sizeof(size_t));

    char* p2 = malloc(size_parametro2);
    memcpy(p2, stream+sizeof(size_t), size_parametro2);
    *parametro2 = p2;

    memcpy(parametro1, stream+sizeof(size_t)+size_parametro2, sizeof(uint32_t));
}
bool recv_WRITE_CPU(int fd,uint32_t* parametro1,char** parametro2) {

    size_t size_payload;
    if (recv(fd, &size_payload, sizeof(size_t), 0) != sizeof(size_t))
        return false;

    void* stream = malloc(size_payload);
    if (recv(fd, stream, size_payload, 0) != size_payload) {
        free(stream);
        return false;
    }

    deserializar_WRITE_CPU(stream, parametro2, parametro1);

    free(stream);
    return true;
}
bool send_WRITE_CPU(int fd,uint32_t parametro1 , char* parametro2) {
    size_t size;
    void* stream = serializar_WRITE_CPU(&size, parametro2, parametro1);
    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

static void* serializar_WRITE_FS(size_t* size, char* parametro2, uint32_t parametro1) {
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
    op_code cop = WRITE_FS;
    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream+sizeof(op_code), &size_payload, sizeof(size_t));
    memcpy(stream+sizeof(op_code)+sizeof(size_t), &size_parametro2, sizeof(size_t));
    memcpy(stream+sizeof(op_code)+sizeof(size_t)*2, parametro2, size_parametro2);
    memcpy(stream+sizeof(op_code)+sizeof(size_t)*2+size_parametro2, &parametro1, sizeof(uint32_t));

    return stream;
}
static void deserializar_WRITE_FS(void* stream, char** parametro2, uint32_t* parametro1) {
	// Carga los parametros al reves pero funciona como deberia funcionar
	size_t size_parametro2;
    memcpy(&size_parametro2, stream, sizeof(size_t));

    char* p2 = malloc(size_parametro2);
    memcpy(p2, stream+sizeof(size_t), size_parametro2);
    *parametro2 = p2;

    memcpy(parametro1, stream+sizeof(size_t)+size_parametro2, sizeof(uint32_t));
}
bool recv_WRITE_FS(int fd,uint32_t* parametro1,char** parametro2) {

    size_t size_payload;
    if (recv(fd, &size_payload, sizeof(size_t), 0) != sizeof(size_t))
        return false;

    void* stream = malloc(size_payload);
    if (recv(fd, stream, size_payload, 0) != size_payload) {
        free(stream);
        return false;
    }

    deserializar_WRITE_FS(stream, parametro2, parametro1);

    free(stream);
    return true;
}
bool send_WRITE_FS(int fd,uint32_t parametro1 , char* parametro2) {
    size_t size;
    void* stream = serializar_WRITE_FS(&size, parametro2, parametro1);
    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}


static void* serializar_READ_FS(uint32_t parametro1, uint32_t parametro2) {
    void* stream = malloc(sizeof(op_code) + sizeof(uint32_t) * 2);
    op_code cop = READ_FS;
    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream+sizeof(op_code), &parametro1, sizeof(uint32_t));
    memcpy(stream+sizeof(op_code)+sizeof(uint32_t), &parametro2, sizeof(uint32_t));
    return stream;
}
static void deserializar_READ_FS(void* stream, uint32_t* parametro1, uint32_t* parametro2) {
    memcpy(parametro1, stream, sizeof(uint32_t));
    memcpy(parametro2, stream+sizeof(uint32_t), sizeof(uint32_t));
}
bool recv_READ_FS(int socket_cliente, uint32_t* parametro1,uint32_t* parametro2){
	 size_t size = sizeof(uint32_t) * 2;
	    void* stream = malloc(size);
	    if (recv(socket_cliente, stream, size, 0) != size) {
	        free(stream);
	        return false;
	    }
	    deserializar_READ_FS(stream, parametro1, parametro2);
	    free(stream);
		return true;
}
bool send_READ_FS(int socket_cliente, uint32_t  parametro1,uint32_t  parametro2){
    size_t size = sizeof(op_code) + sizeof(uint32_t) * 2;
    void* stream = serializar_READ_FS(parametro1, parametro2);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

static void* serializar_READ_CPU(uint32_t parametro1, uint32_t parametro2) {
    void* stream = malloc(sizeof(op_code) + sizeof(uint32_t) * 2);
    op_code cop = READ_CPU;
    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream+sizeof(op_code), &parametro1, sizeof(uint32_t));
    memcpy(stream+sizeof(op_code)+sizeof(uint32_t), &parametro2, sizeof(uint32_t));
    return stream;
}
static void deserializar_READ_CPU(void* stream, uint32_t* parametro1, uint32_t* parametro2) {
    memcpy(parametro1, stream, sizeof(uint32_t));
    memcpy(parametro2, stream+sizeof(uint32_t), sizeof(uint32_t));
}
bool recv_READ_CPU(int socket_cliente, uint32_t* parametro1,uint32_t* parametro2){
	 size_t size = sizeof(uint32_t) * 2;
	    void* stream = malloc(size);
	    if (recv(socket_cliente, stream, size, 0) != size) {
	        free(stream);
	        return false;
	    }
	    deserializar_READ_CPU(stream, parametro1, parametro2);
	    free(stream);
		return true;
}
bool send_READ_CPU(int socket_cliente, uint32_t  parametro1,uint32_t  parametro2){
    size_t size = sizeof(op_code) + sizeof(uint32_t) * 2;
    void* stream = serializar_READ_CPU(parametro1, parametro2);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}









static void* serializar_direccion_fisica(uint32_t parametro1) {
    void* stream = malloc( sizeof(uint32_t));
    memcpy(stream, &parametro1, sizeof(uint32_t));
    return stream;
}
static void deserializar_direccion_fisica(void* stream, uint32_t* parametro1) {
    memcpy(parametro1, stream, sizeof(uint32_t));
}
bool recv_direccion_fisica(int socket_cliente, uint32_t* parametro1){
    size_t size = sizeof(uint32_t);
    void* stream = malloc(size);
    if (recv(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    deserializar_direccion_fisica(stream, parametro1);
    free(stream);
	return true;
}
bool send_direccion_fisica (int socket_cliente, uint32_t  parametro1){
    size_t size = sizeof(uint32_t);
    void* stream = serializar_direccion_fisica(parametro1);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}


static void* serializar_tamanio_a_leer(uint32_t parametro1) {
   void* stream = malloc(sizeof(op_code) + sizeof(uint32_t));
   op_code cop = READ;
    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream+sizeof(op_code), &parametro1, sizeof(uint32_t));
    return stream;
}
static void deserializar_tamanio_a_leer(void* stream, uint32_t* parametro1) {
    memcpy(parametro1, stream ,sizeof(uint32_t));
}
bool recv_tamanio_a_leer(int socket_cliente, uint32_t* parametro1){
    size_t size = sizeof(uint32_t);
    void* stream = malloc(size);
    if (recv(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    deserializar_tamanio_a_leer(stream, parametro1);
    free(stream);
	return true;
}
bool send_tamanio_a_leer(int socket_cliente, uint32_t  parametro1){
    size_t size = sizeof(op_code) + sizeof(uint32_t);
    void* stream = serializar_tamanio_a_leer(parametro1);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

static void* serializar_cant_bytes(uint32_t parametro1) {
    void* stream = malloc( sizeof(uint32_t));
    memcpy(stream, &parametro1, sizeof(uint32_t));
    return stream;
}
static void deserializar_cant_bytes(void* stream, uint32_t* parametro1) {
    memcpy(parametro1, stream, sizeof(uint32_t));
}
bool recv_cant_bytes(int socket_cliente, uint32_t* parametro1){
    size_t size = sizeof(uint32_t);
    void* stream = malloc(size);
    if (recv(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    deserializar_cant_bytes(stream, parametro1);
    free(stream);
	return true;
}
bool send_cant_bytes(int socket_cliente, uint32_t  parametro1){
    size_t size = sizeof(uint32_t);
    void* stream = serializar_cant_bytes(parametro1);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

static void* serializar_contenido_leido(size_t* size,char* recurso){
		size_t size_recurso = strlen(recurso)+1;
		*size =2*sizeof(size_t)+ size_recurso;
		size_t size_payload = *size- sizeof(size_t);
		void* stream = malloc(*size);
		    memcpy(stream, &size_payload, sizeof(size_t));
		    memcpy(stream+sizeof(size_t), &size_recurso, sizeof(size_t));
		    memcpy(stream+sizeof(size_t)+sizeof(size_t), recurso, size_recurso);
	// copio al stream en orden cop,payload,sizep2,p2
		    return stream;

}
static void deserializar_contenido_leido(void* stream,char** recurso){
	size_t size_archivo;
	memcpy(&size_archivo, stream, sizeof(size_t));
	char* p2 = malloc(size_archivo);
	*recurso = p2;
	memcpy(p2,stream+sizeof(size_t) ,size_archivo);
// OPCODE,PAYLOAD, SIZE P2, P2
}
bool recv_contenido_leido(int socket_cliente, char** recurso){
	 size_t size_payload ;
	    if (recv(socket_cliente,&size_payload,sizeof(size_t), 0) != sizeof(size_t)) {
	        return false;
	    }
	    void* stream = malloc(size_payload);

	    if (recv(socket_cliente,stream ,size_payload, 0) != size_payload) {
	        free(stream);
	        return false;
	    }
	    deserializar_contenido_leido(stream, recurso);
	    free(stream);
	    return true;
}
bool send_contenido_leido(int socket_cliente, char* recurso){
    size_t size;
    void* stream = serializar_contenido_leido(&size ,recurso);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}
static void* serializar_READ(uint32_t parametro1, uint32_t parametro2) {
    void* stream = malloc(sizeof(op_code) + sizeof(uint32_t) * 2);
    op_code cop = READ;
    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream+sizeof(op_code), &parametro1, sizeof(uint32_t));
    memcpy(stream+sizeof(op_code)+sizeof(uint32_t), &parametro2, sizeof(uint32_t));
    return stream;
}
static void deserializar_READ(void* stream, uint32_t* parametro1, uint32_t* parametro2) {
    memcpy(parametro1, stream, sizeof(uint32_t));
    memcpy(parametro2, stream+sizeof(uint32_t), sizeof(uint32_t));
}
bool recv_READ(int socket_cliente, uint32_t* parametro1,uint32_t* parametro2){
	 size_t size = sizeof(uint32_t) * 2;
	    void* stream = malloc(size);
	    if (recv(socket_cliente, stream, size, 0) != size) {
	        free(stream);
	        return false;
	    }
	    deserializar_READ(stream, parametro1, parametro2);
	    free(stream);
		return true;
}
bool send_READ(int socket_cliente, uint32_t  parametro1,uint32_t  parametro2){
    size_t size = sizeof(op_code) + sizeof(uint32_t) * 2;
    void* stream = serializar_READ(parametro1, parametro2);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

static void* serializar_PUNTERO_FS(uint32_t parametro1) {
    void* stream = malloc( sizeof(uint32_t));
    memcpy(stream, &parametro1, sizeof(uint32_t));
    return stream;
}
static void deserializar_PUNTERO_FS(void* stream, uint32_t* parametro1) {
    memcpy(parametro1, stream, sizeof(uint32_t));
}
bool recv_PUNTERO_FS(int socket_cliente, uint32_t* parametro1){
    size_t size = sizeof(uint32_t);
    void* stream = malloc(size);
    if (recv(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    deserializar_PUNTERO_FS(stream, parametro1);
    free(stream);
	return true;
}
bool send_PUNTERO_FS(int socket_cliente, uint32_t  parametro1){
    size_t size = sizeof(uint32_t);
    void* stream = serializar_PUNTERO_FS(parametro1);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}


static void* serializar_ERROR() {
   void* stream = malloc(sizeof(op_code));
    op_code cop = ERROR;
    memcpy(stream, &cop, sizeof(op_code));
    return stream;
}
bool recv_ERROR(int socket_cliente){

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
bool send_ERROR(int socket_cliente){
   size_t size = sizeof(op_code);
    void* stream = serializar_ERROR();
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    free(stream);
    return true;
}

static void* serializar_LEER_CONTENIDO_CPU() {
   void* stream = malloc(sizeof(op_code));
    op_code cop = LEER_CPU;
    memcpy(stream, &cop, sizeof(op_code));
    return stream;
}
bool recv_LEER_CONTENIDO_CPU(int socket_cliente){
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
bool send_LEER_CONTENIDO_CPU(int socket_cliente){
   size_t size = sizeof(op_code);
    void* stream = serializar_LEER_CONTENIDO_CPU();
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    free(stream);
    return true;
}
//static void* serializar_CONTEXTO_EJECUCION(contexto_ejecucion* contexto, uint32_t* tamanio_serializado) {
//    uint32_t num_elementos = contexto->TSegmento->elements_count;
//    size_t size_segmentos = sizeof(segmento_t) * num_elementos;
//    *tamanio_serializado = sizeof(uint32_t) * 2 + sizeof(registros_cpu) + size_segmentos;
//    void* stream = malloc(*tamanio_serializado);
//    void* offset = stream;
//
//    memcpy(offset, &(contexto->PID), sizeof(uint32_t));
//    offset += sizeof(uint32_t);
//    memcpy(offset, &(contexto->PC), sizeof(uint32_t));
//    offset += sizeof(uint32_t);
//    memcpy(offset, &(contexto->registros), sizeof(registros_cpu));
//    offset += sizeof(registros_cpu);
//    memcpy(offset, &(contexto->TSegmento->elements_count), size_segmentos);
//
//    return stream;
//}
//
//static void deserializar_CONTEXTO_EJECUCION(void* stream, contexto_ejecucion* contexto) {
//    void* offset = stream;
//
//    memcpy(&(contexto->PID), offset, sizeof(uint32_t));
//    offset += sizeof(uint32_t);
//    memcpy(&(contexto->PC), offset, sizeof(uint32_t));
//    offset += sizeof(uint32_t);
//    memcpy(&(contexto->registros), offset, sizeof(registros_cpu));
//    offset += sizeof(registros_cpu);
//    uint32_t num_elementos = contexto->TSegmento->elements_count;
//    for (int i = 0; i < num_elementos; i++) {
//           segmento_t* segmento = malloc(sizeof(segmento_t));
//           memcpy(segmento, offset, sizeof(segmento_t));
//           list_add(contexto->TSegmento, segmento);
//           offset += sizeof(segmento_t);
//       }
//}
//// Función send_contexto_ejecucion
//bool send_CONTEXTO_EJECUCION(int socket_cliente, contexto_ejecucion contexto) {
//    //uint32_t num_elementos = contexto.TSegmento->elements_count;
//    uint32_t tamanio_serializado;
//    void* stream = serializar_CONTEXTO_EJECUCION(&contexto, &tamanio_serializado);
//
//    // Enviar el tamaño del stream serializado
//    if (send(socket_cliente, &tamanio_serializado, sizeof(uint32_t), 0) != sizeof(uint32_t)) {
//        free(stream);
//        return false;
//    }
//
//    // Enviar el stream serializado
//    if (send(socket_cliente, stream, tamanio_serializado, 0) != tamanio_serializado) {
//        free(stream);
//        return false;
//    }
//
//    // Enviar la tabla de segmentos
//    if (!send_TABLA_SEGMENTOS(socket_cliente, contexto.TSegmento)) {
//        free(stream);
//        return false;
//    }
//
//    // Liberar la memoria asignada
//    free(stream);
//    return true;
//}
//
//bool recv_CONTEXTO_EJECUCION(int socket_cliente, contexto_ejecucion* contexto) {
//    uint32_t tamanio_serializado;
//    if (recv(socket_cliente, &tamanio_serializado, sizeof(uint32_t), 0) != sizeof(uint32_t)) {
//        return false;
//    }
//
//    void* stream = malloc(tamanio_serializado);
//
//    // Recibir el stream serializado
//    if (recv(socket_cliente, stream, tamanio_serializado, 0) != tamanio_serializado) {
//        free(stream);
//        return false;
//    }
//
//    // Recibir la tabla de segmentos
//    if (!recv_TABLA_SEGMENTOS(socket_cliente, &(contexto->TSegmento))) {
//        free(stream);
//        return false;
//    }
//
//    // Deserializar el contexto de ejecución
//    deserializar_CONTEXTO_EJECUCION(stream, contexto);
//
//    // Liberar la memoria asignada
//    free(stream);
//    return true;
//}

static void* serializar_seguir_ejecutando(uint32_t parametro1) {
   void* stream = malloc(sizeof(op_code) + sizeof(uint32_t));
   op_code cop = SEGUI_EJECUTANDO;
    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream+sizeof(op_code), &parametro1, sizeof(uint32_t));
    return stream;
}
static void deserializar_seguir_ejecutando(void* stream, uint32_t* parametro1) {
    memcpy(parametro1, stream ,sizeof(uint32_t));
}
uint32_t recv_seguir_ejecutando(int socket_cliente){
    uint32_t parametro1;
    size_t size = sizeof(uint32_t);
    void* stream = malloc(size);
    if (recv(socket_cliente, stream, size, 0) != size) {
        free(stream);

    }
    deserializar_seguir_ejecutando(stream, &parametro1);
    free(stream);
	return parametro1;
}
bool send_seguir_ejecutando(int socket_cliente, uint32_t  parametro1){
    size_t size = sizeof(op_code) + sizeof(uint32_t);
    void* stream = serializar_seguir_ejecutando(parametro1);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}
static void* serializar_CONTEXTO_EJECUCION(contexto_ejecucion* contexto) {
    size_t size = sizeof(uint32_t) * 2 + sizeof(registros_cpu);
    void* stream = malloc(size);
    memcpy(stream, &(contexto->PID), sizeof(uint32_t));
    memcpy(stream + sizeof(uint32_t), &(contexto->PC), sizeof(uint32_t));
    memcpy(stream + sizeof(uint32_t) * 2, &(contexto->registros), sizeof(registros_cpu));
    return stream;
}

static void deserializar_CONTEXTO_EJECUCION(void* stream, contexto_ejecucion* contexto) {
    memcpy(&(contexto->PID), stream, sizeof(uint32_t));
    memcpy(&(contexto->PC), stream + sizeof(uint32_t), sizeof(uint32_t));
    memcpy(&(contexto->registros), stream + sizeof(uint32_t) * 2, sizeof(registros_cpu));
}

// Función send_contexto_ejecucion
bool send_CONTEXTO_EJECUCION(int socket_cliente, contexto_ejecucion contexto) {
    // Serializar el contexto de ejecución
    void* stream = serializar_CONTEXTO_EJECUCION(&contexto);
    size_t size = sizeof(uint32_t) * 2 + sizeof(registros_cpu);

    // Enviar el tamaño del stream serializado
    if (send(socket_cliente, &size, sizeof(size_t), 0) != sizeof(size_t)) {
        free(stream);
        return false;
    }

    // Enviar el stream serializado
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    // Liberar la memoria asignada
    free(stream);
    return true;
}

bool recv_CONTEXTO_EJECUCION(int socket_cliente, contexto_ejecucion* contexto) {
    size_t size;
    if (recv(socket_cliente, &size, sizeof(size_t), 0) != sizeof(size_t)) {
        return false;
    }

    void* stream = malloc(size);

    // Recibir el stream serializado
    if (recv(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    // Deserializar el contexto de ejecución
    deserializar_CONTEXTO_EJECUCION(stream, contexto);

    // Liberar la memoria asignada
    free(stream);
    return true;
}
