#include "send_rcvs.h"
static void* serializar_SET(size_t* size, uint32_t  parametro1, char* parametro2 ){
	size_t size_parametro2 = strlen(parametro2) +1; // No sabemos si agregar +1 PREGUNTAR
	*size =sizeof(op_code) + 2*sizeof(size_t) + sizeof(uint32_t)+ size_parametro2;
	size_t size_payload = *size - sizeof(op_code) - sizeof(size_t);
	void* stream = malloc(*size);
	op_code cop = SET;
	    memcpy(stream, &cop, sizeof(op_code));
	    memcpy(stream+sizeof(op_code), &size_payload, sizeof(size_t));
	    memcpy(stream+sizeof(op_code)+sizeof(size_t), &parametro1,sizeof(uint32_t));
	    memcpy(stream+sizeof(op_code)+sizeof(size_t)+sizeof(uint32_t), &size_parametro2, sizeof(size_t));
	    memcpy(stream+sizeof(op_code)+sizeof(size_t)+sizeof(uint32_t) + sizeof(size_t), &parametro2, size_parametro2);
// copio al stream en orden cop,payload,p1,sizep2,p2
// Creo que esta mal, primero le copiamos opcode con size opcode, despues copiamos payload que es tamanio size_t
// despues copio el parametro1 que es tamanio uint32_t, despues copio el size de p2, con tamanio size_t,
// ahora le sumo el offset de otro size_t , copio el parametrto2, cont amanio size p2 YA LO CAMBIE PERO HABLARLO
	    return stream;
}
static void deserializar_SET(void* stream,uint32_t* parametro1 ,char** parametro2){
	size_t size_parametro2;
	memcpy(&parametro1,stream,sizeof(uint32_t));

	memcpy(&size_parametro2, stream +sizeof(uint32_t), sizeof(size_t));
	char* p2 = malloc(size_parametro2);

	memcpy(p2,stream+sizeof(uint32_t)+sizeof(size_t) ,size_parametro2);
	*parametro2 = p2;
// OPCODE,PAYLOAD,ENTERO P1, SIZE P2, P2
}
bool recv_SET(int socket_cliente,uint32_t*  parametro1, char** parametro2){
    size_t size_payload ;
// Cuando recibo la instruccion set, voy a tener el op_code(Cosa que ya recibi antes de llegar aca),
// ahora recibo el payload (Que es el tamanio de los parametros que mando),
// seguidos por el entero y el char.

    if (recv(socket_cliente,&size_payload,sizeof(size_t), 0) != sizeof(size_t)) {
        return false;
    }
    void* stream = malloc(size_payload);

    if (recv(socket_cliente,stream ,size_payload, 0) != size_payload) {
        free(stream);
        return false;
    }
   deserializar_SET(stream, parametro1, parametro2);
    free(stream);
    return true;

}
bool send_SET(int socket_cliente, uint32_t  parametro1, char* parametro2){
    size_t size;
    void* stream = serializar_SET(&size ,parametro1, parametro2);
    if (send(socket_cliente, stream, size, 0) != size) {
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
		    memcpy(stream+sizeof(op_code)+sizeof(size_t)+sizeof(size_t), &recurso, size_recurso);
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
		    memcpy(stream+sizeof(op_code)+sizeof(size_t)+sizeof(size_t), &recurso, size_recurso);
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
		    memcpy(stream+sizeof(op_code)+sizeof(size_t)+size_archivo, &archivo, size_archivo);
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
	    memcpy(stream+sizeof(op_code)+sizeof(size_t)+sizeof(size_t), &parametro1, size_parametro1);
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
	op_code cop = F_TRUNCATE;
	    memcpy(stream, &cop, sizeof(op_code));
	    memcpy(stream+sizeof(op_code), &size_payload, sizeof(size_t));
	    memcpy(stream+sizeof(op_code)+sizeof(size_t), &size_parametro1,sizeof(size_t));
	    memcpy(stream+sizeof(op_code)+sizeof(size_t)+sizeof(size_t), &parametro1, size_parametro1);
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
	op_code cop = F_TRUNCATE;
	    memcpy(stream, &cop, sizeof(op_code));
	    memcpy(stream+sizeof(op_code), &size_payload, sizeof(size_t));
	    memcpy(stream+sizeof(op_code)+sizeof(size_t), &size_parametro1,sizeof(size_t));
	    memcpy(stream+sizeof(op_code)+sizeof(size_t)+sizeof(size_t), &parametro1, size_parametro1);
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
	op_code cop = F_TRUNCATE;
	    memcpy(stream, &cop, sizeof(op_code));
	    memcpy(stream+sizeof(op_code), &size_payload, sizeof(size_t));
	    memcpy(stream+sizeof(op_code)+sizeof(size_t), &size_parametro1,sizeof(size_t));
	    memcpy(stream+sizeof(op_code)+sizeof(size_t)+sizeof(size_t), &parametro1, size_parametro1);
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
		op_code cop = F_OPEN;
		    memcpy(stream, &cop, sizeof(op_code));
		    memcpy(stream+sizeof(op_code), &size_payload, sizeof(size_t));
		    memcpy(stream+sizeof(op_code)+sizeof(size_t), &size_archivo, sizeof(size_t));
		    memcpy(stream+sizeof(op_code)+sizeof(size_t)+size_archivo, &archivo, size_archivo);
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
static void* serializar_INICIAR_ESTRUCTURA_MEMORIA(size_t* size, char* mensaje){
	size_t size_mensaje = strlen(mensaje)+1;
		*size = 2*sizeof(size_t)+ size_mensaje;
		size_t size_payload = *size - sizeof(op_code) - sizeof(size_t);
		void* stream = malloc(*size);
		memcpy(stream, &size_payload, sizeof(size_t));
		memcpy(stream+sizeof(size_t), &size_mensaje, sizeof(size_t));
		memcpy(stream+sizeof(size_t)+sizeof(size_t), &mensaje, size_mensaje);
		return stream;
}
static void deserializar_INICIAR_ESTRUCTURA_MEMORIA(void* stream,char** mensaje){
	size_t size_mensaje;
	memcpy(&size_mensaje, stream, sizeof(size_t));
	char* p2 = malloc(size_mensaje);
	*mensaje = p2;
	memcpy(p2,stream+sizeof(size_t) ,size_mensaje);
// OPCODE,PAYLOAD, SIZE P2, P2
}

void send_INICIAR_ESTRUCTURA_MEMORIA(int server_memoria,char* mensaje){
	size_t size;
    void* stream = serializar_INICIAR_ESTRUCTURA_MEMORIA(&size ,mensaje);
    if (send(server_memoria, stream, size, 0) != size) {
        free(stream);
    }
    free(stream);
}
bool recv_INICIAR_ESTRUCTURA_MEMORIA(int socket_cliente, char** mensaje){
	 size_t size_payload ;

	    if (recv(socket_cliente,&size_payload,sizeof(size_t), 0) != sizeof(size_t)) {
	        return false;
	    }
	    void* stream = malloc(size_payload);

	    if (recv(socket_cliente,stream ,size_payload, 0) != size_payload) {
	        free(stream);
	        return false;
	    }
	    deserializar_INICIAR_ESTRUCTURA_MEMORIA(stream, mensaje);
	    free(stream);
	    return true;
}

static void* serializar_TABLA_SEGMENTOS(size_t* size, tabla_segmentos* tabla) {
    *size = sizeof(uint32_t) * 3;
    void* stream = malloc(*size);
    memcpy(stream, &tabla->id, sizeof(uint32_t));
    memcpy(stream + sizeof(uint32_t), &tabla->direccion_Base, sizeof(uint32_t));
    memcpy(stream + 2 * sizeof(uint32_t), &tabla->tamanio, sizeof(uint32_t));
    return stream;
}
static void deserializar_TABLA_SEGMENTOS(void* stream,tabla_segmentos** tabla){
    tabla_segmentos* table = malloc(sizeof(tabla_segmentos));
    memcpy(&table->id, stream, sizeof(uint32_t));
    memcpy(&table->direccion_Base, stream + sizeof(uint32_t), sizeof(uint32_t));
    memcpy(&table->tamanio, stream + 2*sizeof(uint32_t), sizeof(uint32_t));
    *tabla = table;
}
bool send_TABLA_SEGMENTOS(int server_memoria, tabla_segmentos* tabla){
	size_t size;
    void* stream = serializar_TABLA_SEGMENTOS(&size ,tabla);
    if (send(server_memoria, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;

}
bool recv_TABLA_SEGMENTOS(int socket_cliente, tabla_segmentos** tabla){
	    size_t size = sizeof(uint32_t) * 3;
	    void* stream = malloc(size);

	    if (recv(socket_cliente, stream, size, 0) != size) {
	        free(stream);
	        return false;
	    }
	    deserializar_TABLA_SEGMENTOS(stream, tabla);
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


void deserializar_PC(void* stream, uint32_t* parametro1) {
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


void deserializar_tiempo_bloqueante(void* stream, uint32_t* parametro1) {
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



/* static void* serializar_reg_cpu(registros_cpu* reg) {

}// TODO
static void deserializar_reg_cpu(void* stream, registros_cpu* reg) {


}// TODO
bool send_reg_cpu(int fd_cpu, registros_cpu* reg){


	return true;
}// TODO
bool recv_reg_cpu(fd_cpu, pcb_proceso->registros){


	return true;
}// TODO
*/
