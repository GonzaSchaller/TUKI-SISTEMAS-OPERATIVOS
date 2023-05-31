#ifndef SRC_ESTRUCTURAS_COMPARTIDAS_H_
#define SRC_ESTRUCTURAS_COMPARTIDAS_H_
#include <commons/collections/queue.h>

typedef union {//porque puedo recibir int o char*
	uint32_t tipo_int;
	char* tipo_string;
 }tipo_parametro;


typedef struct{
	int id;
	char* nombre; //set
	tipo_parametro parametro1;
	tipo_parametro parametro2;
	uint32_t parametro3;
}instruccion;

typedef struct{
    uint32_t id;
    uint32_t direccion_Base;
    uint32_t tamanio;
}segmento_t;

typedef enum{
   	 EXITOSO,
   	 FALLIDO,
   	 COMPACTAR
} estados_segmentos;

typedef struct{ //A implmentar por FileSystem
	char* nombreArchivo;
	uint32_t puntero_directo;
	uint32_t puntero_indirecto;
	//... algo mas
	t_queue* colaBloqueados;
	pthread_mutex_t mutexArchivo; //para garantizar mutua exclusion //todo ver donde inicializarlo

} fcb_t;

typedef struct{
	   char AX[4]; // Registro AX de 4 bytes
	   char BX[4];
	   char CX[4];
	   char DX[4];
	   char EAX[8]; // Registro EAX de 8 bytes
	   char EBX[8];
	   char ECX[8];
	   char EDX[8];
	   char RAX[16]; // Registro RAX de 16 bytes
	   char RBX[16];
	   char RCX[16];
	   char RDX[16];
}registros_cpu;

typedef struct{
    uint32_t PID;
    uint32_t PC;
    registros_cpu registros;
    t_list* TSegmento;

}contexto_ejecucion;

typedef enum{ 	//parametros // a partir del numero 100, son instrucciones
	SET = 100,  // 2
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








#endif /* SRC_ESTRUCTURAS_COMPARTIDAS_H_ */
