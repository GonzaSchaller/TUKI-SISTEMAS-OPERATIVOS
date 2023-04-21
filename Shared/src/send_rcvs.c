#include "send_rcvs.h"



bool recv_SET(int socket_cliente, uint32_t* parametro1, uint32_t* parametro2){
    size_t size = sizeof(uint32_t) * 2;
    void* stream = malloc(size);

    if (recv(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    deserializar_SET(stream, parametro1, parametro2);

    free(stream);
    return true;

}
bool send_SET(int socket_cliente, uint32_t  parametro1, uint32_t  parametro2){
	//printf("Entre en send_COPY \n");
    size_t size = sizeof(op_code) + sizeof(uint32_t) * 2;
    void* stream = serializar_SET(parametro1, parametro2);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}
static void* serializar_SET(uint32_t parametro1,uint32_t parametro2) {
   void* stream = malloc(sizeof(op_code) + sizeof(uint32_t));

   op_code cop = SET;
    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream+sizeof(op_code), &parametro1, sizeof(uint32_t));

    //printf("El cop en serializar NO_OP es: %d\n",cop);
   // printf("El parametro 1 es: %d\n",parametro1);
    //printf("El tam del stream cuando lo serializamos es %d\n", sizeof(stream));
    return stream;
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

bool recv_WAIT(int socket_cliente, uint32_t* parametro1){
    size_t size = sizeof(uint32_t);
    void* stream = malloc(size);

    if (recv(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    deserializar_WAIT(stream, parametro1);

    free(stream);
    return true;

}
bool send_WAIT(int socket_cliente, uint32_t  parametro1){
	//printf("Entre en send_COPY \n");
    size_t size = sizeof(op_code) + sizeof(uint32_t);
    void* stream = serializar_WAIT(parametro1);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
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
	//printf("Entre en send_COPY \n");
    size_t size = sizeof(op_code) + sizeof(uint32_t);
    void* stream = serializar_IO(parametro1);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

bool recv_SIGNAL(int socket_cliente, uint32_t* parametro1){
    size_t size = sizeof(uint32_t);
    void* stream = malloc(size);

    if (recv(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    deserializar_SIGNAL(stream, parametro1);

    free(stream);
    return true;

}
bool send_SIGNAL(int socket_cliente, uint32_t  parametro1){
	//printf("Entre en send_COPY \n");
    size_t size = sizeof(op_code) + sizeof(uint32_t);
    void* stream = serializar_WAIT(parametro1);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
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
	//printf("Entre en send_COPY \n");
    size_t size = sizeof(op_code) + sizeof(uint32_t) * 2;
    void* stream = serializar_MOV_IN(parametro1, parametro2);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

bool recv_F_OPEN(int socket_cliente, uint32_t* parametro1){
    size_t size = sizeof(uint32_t);
    void* stream = malloc(size);

    if (recv(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    deserializar_F_OPEN(stream, parametro1);

    free(stream);
    return true;

}
bool send_F_OPEN(int socket_cliente, uint32_t  parametro1){
	//printf("Entre en send_COPY \n");
    size_t size = sizeof(op_code) + sizeof(uint32_t);
    void* stream = serializar_F_OPEN(parametro1);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}


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
   size_t size = sizeof(op_code);

    void* stream = serializar_EXIT();


    if (send(socket_cliente, stream, size, 0) != size) { //ACA EN EL SEND PUSE EL EXIT, ANTES HABIA UN STREAM
      //  free(stream);
        return false;
    }

   // free(stream);
    return true;
}

bool recv_F_TRUNCATE(int socket_cliente, uint32_t* parametro1,uint32_t* parametro2){
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
bool send_F_TRUNCATE(int socket_cliente, uint32_t  parametro1,uint32_t  parametro2){
	//printf("Entre en send_COPY \n");
    size_t size = sizeof(op_code) + sizeof(uint32_t) * 2;
    void* stream = serializar_F_TRUNCATE(parametro1, parametro2);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

bool recv_F_SEEK(int socket_cliente, uint32_t* parametro1,uint32_t* parametro2){
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
bool send_F_SEEK(int socket_cliente, uint32_t  parametro1,uint32_t  parametro2){
	//printf("Entre en send_COPY \n");
    size_t size = sizeof(op_code) + sizeof(uint32_t) * 2;
    void* stream = serializar_F_SEEK(parametro1, parametro2);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
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
	//printf("Entre en send_COPY \n");
    size_t size = sizeof(op_code) + sizeof(uint32_t) * 2;
    void* stream = serializar_CREATE_SEGMENT(parametro1, parametro2);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

bool recv_F_WRITE(int socket_cliente, uint32_t* parametro1,uint32_t* parametro2, uint32_t* parametro3){
	 size_t size = sizeof(uint32_t) * 3;
	    void* stream = malloc(size);

	    if (recv(socket_cliente, stream, size, 0) != size) {
	        free(stream);
	        return false;
	    }

	    deserializar_F_WRITE(stream, parametro1, parametro2, parametro3);

	    free(stream);
		return true;

}
bool send_F_WRITE(int socket_cliente, uint32_t  parametro1,uint32_t  parametro2, uint32_t* parametro3){
	//printf("Entre en send_COPY \n");
    size_t size = sizeof(op_code) + sizeof(uint32_t) * 3;
    void* stream = serializar_F_WRITE(parametro1, parametro2,parametro3);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

bool recv_F_READ(int socket_cliente, uint32_t* parametro1,uint32_t* parametro2, uint32_t* parametro3){
	 size_t size = sizeof(uint32_t) * 3;
	    void* stream = malloc(size);

	    if (recv(socket_cliente, stream, size, 0) != size) {
	        free(stream);
	        return false;
	    }

	    deserializar_F_READ(stream, parametro1, parametro2, parametro3);

	    free(stream);
		return true;

}
bool send_F_READ(int socket_cliente, uint32_t  parametro1,uint32_t  parametro2, uint32_t* parametro3){
	//printf("Entre en send_COPY \n");
    size_t size = sizeof(op_code) + sizeof(uint32_t) * 3;
    void* stream = serializar_F_READ(parametro1, parametro2, parametro3);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
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
	return true;
}
bool send_DELETE_SEGMENT(int socket_cliente, uint32_t  parametro1){
	//printf("Entre en send_COPY \n");
    size_t size = sizeof(op_code) + sizeof(uint32_t);
    void* stream = serializar_DELETE_SEGMENT(parametro1);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

bool recv_F_CLOSE(int socket_cliente, uint32_t* parametro1){
    size_t size = sizeof(uint32_t);
    void* stream = malloc(size);

    if (recv(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    deserializar_F_CLOSE(stream, parametro1);

    free(stream);
    return true;
	return true;
}
bool send_F_CLOSE(int socket_cliente, uint32_t  parametro1){
	//printf("Entre en send_COPY \n");
    size_t size = sizeof(op_code) + sizeof(uint32_t);
    void* stream = serializar_F_CLOSE(parametro1);
    if (send(socket_cliente, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}


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
   size_t size = sizeof(op_code);

    void* stream = serializar_EXIT();


    if (send(socket_cliente, stream, size, 0) != size) { //ACA EN EL SEND PUSE EL EXIT, ANTES HABIA UN STREAM
      //  free(stream);
        return false;
    }

   // free(stream);
    return true;
}


//static void* serializar_SET(uint32_t parametro1, uint32_t parametro2) {
//    void* stream = malloc(sizeof(op_code) + sizeof(uint32_t) * 2);
//
//    op_code cop = SET;
//    memcpy(stream, &cop, sizeof(op_code));
//    memcpy(stream+sizeof(op_code), &parametro1, sizeof(uint32_t));
//    memcpy(stream+sizeof(op_code)+sizeof(uint32_t), &parametro2, sizeof(uint32_t));
//
////    printf("El cop en serializar COPY es: %d\n",cop);
////    printf("El parametro 1 es: %d\n",parametro1);
////    printf("El parametro 2 es: %d\n",parametro2);
//  //  printf("El tam del stream cuando lo serializamos es %d\n", sizeof(stream));
//
//    return stream;
//}
//static void* serializar2parametros(op_code instruccion, uint32_t parametro1, uint32_t parametro2) {
//    void* stream = malloc(sizeof(op_code) + sizeof(uint32_t) * 2);
//
//    op_code cop = instruccion;
//    memcpy(stream, &cop, sizeof(op_code));
//    memcpy(stream+sizeof(op_code), &parametro1, sizeof(uint32_t));
//    memcpy(stream+sizeof(op_code)+sizeof(uint32_t), &parametro2, sizeof(uint32_t));
//
////    printf("El cop en serializar COPY es: %d\n",cop);
////    printf("El parametro 1 es: %d\n",parametro1);
////    printf("El parametro 2 es: %d\n",parametro2);
//  //  printf("El tam del stream cuando lo serializamos es %d\n", sizeof(stream));
//
//    return stream;
//}
//
//static void* serializar2parametros(op_code instruccion, uint32_t parametro1, uint32_t parametro2) {
//    void* stream = malloc(sizeof(op_code) + sizeof(uint32_t) * 2);
//
//    op_code cop = instruccion;
//    memcpy(stream, &cop, sizeof(op_code));
//    memcpy(stream+sizeof(op_code), &parametro1, sizeof(uint32_t));
//    memcpy(stream+sizeof(op_code)+sizeof(uint32_t), &parametro2, sizeof(uint32_t));
//
////    printf("El cop en serializar COPY es: %d\n",cop);
////    printf("El parametro 1 es: %d\n",parametro1);
////    printf("El parametro 2 es: %d\n",parametro2);
//  //  printf("El tam del stream cuando lo serializamos es %d\n", sizeof(stream));
//
//    return stream;
//}
//
//static void* serializar3parametros(op_code instruccion, uint32_t parametro1, uint32_t parametro2, uint32_t parametro3) {
//    void* stream = malloc(sizeof(op_code) + sizeof(uint32_t) * 2);
//
//    op_code cop = instruccion;
//    memcpy(stream, &cop, sizeof(op_code));
//    memcpy(stream+sizeof(op_code), &parametro1, sizeof(uint32_t));
//    memcpy(stream+sizeof(op_code)+sizeof(uint32_t), &parametro2, sizeof(uint32_t));
//    memcpy(stream+sizeof(op_code)+sizeof(uint32_t)*2, &parametro3, sizeof(uint32_t));
//
////    printf("El cop en serializar COPY es: %d\n",cop);
////    printf("El parametro 1 es: %d\n",parametro1);
////    printf("El parametro 2 es: %d\n",parametro2);
//  //  printf("El tam del stream cuando lo serializamos es %d\n", sizeof(stream));
//
//    return stream;
//}
//




