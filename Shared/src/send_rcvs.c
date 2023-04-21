#include "send_rcvs.h"

bool recv_SET(int socket_cliente,uint32_t  parametro1, char** parametro2){
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
	//printf("Entre en send_COPY \n");
    size_t size;
    void* stream = serializar_SET(&size ,parametro1, parametro2);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}
static void* serializar_SET(size_t* size, uint32_t  parametro1, char* parametro2 ){
	size_t size_parametro2 = strlen(parametro2); // No sabemos si agregar +1 PREGUNTAR
	*size =sizeof(op_code) + 2*sizeof(size_t) + sizeof(uint32_t)+ size_parametro2;
	size_t size_payload = *size - sizeof(op_code) - sizeof(size_t);
	void* stream = malloc(*size);
	op_code cop = SET;
	    memcpy(stream, &cop, sizeof(op_code));
	    memcpy(stream+sizeof(op_code), &size_payload, sizeof(size_t));
	    memcpy(stream+sizeof(op_code)+sizeof(size_t), &parametro1,sizeof(uint32_t));
	    memcpy(stream+sizeof(op_code)+sizeof(size_t)+sizeof(uint32_t), &size_parametro2, sizeof(size_t));
	    memcpy(stream+sizeof(op_code)+sizeof(size_t)+sizeof(uint32_t) + size_parametro2, &parametro2, size_parametro2);
// copio al stream en orden cop,payload,p1,sizep2,p2
	    return stream;
}
static void deserializar_SET(void* stream,uint32_t parametro1 ,char** parametro2){
	size_t size_parametro2;
	memcpy(&parametro1,stream,sizeof(uint32_t));
	memcpy(&size_parametro2, stream +sizeof(uint32_t), sizeof(size_t));
	char* p2 = malloc(size_parametro2);
	*parametro2 = p2;
	memcpy(p2,stream+sizeof(uint32_t)+sizeof(size_t) ,size_parametro2);
// OPCODE,PAYLOAD,ENTERO P1, SIZE P2, P2
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
	//printf("Entre en send_COPY \n");
    size_t size = sizeof(op_code) + sizeof(uint32_t) * 2;
    void* stream = serializar_MOV_OUT(parametro1, parametro2);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}
// FALTA SERIALIZAR Y DESERIALIZAR

bool recv_WAIT(){ // Recursos TODO

    return true;

}
bool send_WAIT(){ // Recursos TODO

    return true;
}
// FALTA TOD0

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
	//printf("Entre en send_COPY \n");
	op_code instruccion = IO;
    size_t size = sizeof(op_code) + sizeof(uint32_t);
    void* stream = serializar_IO(parametro1);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}
// Falta serializar y deserializar

bool recv_SIGNAL(){

	return true;
}
bool send_SIGNAL(){

    return true;
}
// FALTA TOD0, RECIBE RECURSOS

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
	//printf("Entre en send_COPY \n");
	op_code instruccion = SIGNAL;
    size_t size = sizeof(op_code) + sizeof(uint32_t) * 2;
    void* stream = serializar_MOV_IN(parametro1, parametro2);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}
// FALTA SERIALIZAR Y DESERIALIZAR
bool recv_F_OPEN(int socket_cliente, char** archivo){

    return true;

}
bool send_F_OPEN(int socket_cliente, char* archivo){

    return true;
}
// FALTA TOD0
bool recv_YIELD(int socket_cliente){

	    size_t size = sizeof(op_code);
	    void* stream = malloc(size);

	    if (recv(socket_cliente, stream, size, 0) != size) {
	        free(stream);
	        return false;
	    }

	   // deserializar_YIELD(stream);

	    free(stream);
	    return true;

}
bool send_YIELD(int socket_cliente){
	//printf("Entre en send_EXIT \n");
	op_code instruccion = YIELD;
   size_t size = sizeof(op_code);

    void* stream = serializar_EXIT(instruccion);


    if (send(socket_cliente, stream, size, 0) != size) { //ACA EN EL SEND PUSE EL EXIT, ANTES HABIA UN STREAM
      //  free(stream);
        return false;
    }

   // free(stream);
    return true;
}
// DUDOSA

bool recv_F_TRUNCATE(int socket_cliente, char** parametro1,uint32_t* parametro2){

		return true;

}
bool send_F_TRUNCATE(int socket_cliente, char*  parametro1,uint32_t  parametro2){


    return true;
}
// FALTA TOD0

bool recv_F_SEEK(int socket_cliente, char** parametro1,uint32_t* parametro2){

		return true;

}
bool send_F_SEEK(int socket_cliente, char*  parametro1,uint32_t  parametro2){

    return true;
}
// TOD0
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
	//printf("Entre en send_COPY \n");
	op_code instruccion = CREATE_SEGMENT;
    size_t size = sizeof(op_code) + sizeof(uint32_t) * 2;
    void* stream = serializar_CREATE_SEGMENT(parametro1, parametro2);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}
// FALTA SERIALIZAR Y DESERIALIZAR
bool recv_F_WRITE(int socket_cliente, char** parametro1,uint32_t* parametro2, uint32_t* parametro3){

		return true;

}
bool send_F_WRITE(int socket_cliente, char*  parametro1,uint32_t  parametro2, uint32_t* parametro3){
	//printf("Entre en send_COPY \n");

    return true;
}
// FALTA TOD0
bool recv_F_READ(int socket_cliente, char** parametro1,uint32_t* parametro2, uint32_t* parametro3){

		return true;

}
bool send_F_READ(int socket_cliente, char*  parametro1,uint32_t  parametro2, uint32_t* parametro3){

    return true;
}
// FALTA TOD0
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
	return true;
}
bool send_DELETE_SEGMENT(int socket_cliente, uint32_t  parametro1){
	//printf("Entre en send_COPY \n");
	op_code instruccion = DELETE_SEGMENT;
    size_t size = sizeof(op_code) + sizeof(uint32_t);
    void* stream = serializar_DELETE_SEGMENT(parametro1);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}
// FALTA SERIALIZAR Y DESERIALIZAR
bool recv_F_CLOSE(int socket_cliente, char** parametro1){

	return true;
}
bool send_F_CLOSE(int socket_cliente, char*  parametro1){

    return true;
}
// FALTA TOD0

bool recv_EXIT(int socket_cliente) {
    size_t size = sizeof(op_code);
    void* stream = malloc(size);

    if (recv(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }

   // deserializar_EXIT(stream);

    free(stream);
    return true;
}
bool send_EXIT(int socket_cliente) {
	//printf("Entre en send_EXIT \n");
	op_code instruccion = EXIT;
   size_t size = sizeof(op_code);


    void* stream = serializar_EXIT();


    if (send(socket_cliente, stream, size, 0) != size) { //ACA EN EL SEND PUSE EL EXIT, ANTES HABIA UN STREAM
      //  free(stream);
        return false;
    }

   // free(stream);
    return true;
}

// REVISAR YIELD Y EXIT



