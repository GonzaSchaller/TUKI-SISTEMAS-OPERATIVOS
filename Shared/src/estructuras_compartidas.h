#ifndef SRC_ESTRUCTURAS_COMPARTIDAS_H_
#define SRC_ESTRUCTURAS_COMPARTIDAS_H_
#include <commons/collections/queue.h>

typedef enum {
    NEW,
    READY,
    EXEC,
    BLOCK,
    FINISH
}estado;// ESTADO PCB
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

typedef struct{
    t_list* instrucciones; // Lista de instrucciones a ejecutar
    contexto_ejecucion contexto; //tiene registros, tabla segmento, pid y PC
    uint32_t estimacion_prox_rafaga; //S del HRRN
    float horaDeIngresoAReady; // Seria el timestamp en que el proceso llego a ready cambiar nombre
    t_list* tabla_archivosAbiertos ;// lista de fcb_t
    estado state ; //capaz ponerlo uint32
    uint32_t state_anterior;
    float rafaga_anterior_real; // 0 al principio, despues es el tiempo real que se ejecuto. Seria el R(n)
    float estimacion_rafaga_anterior; // estimacion inicial
    float horaDeIngresoAExe; // timestamp cuando llega a execute para calcular la rafaga anterior
	float horaDeSalidaDeExe;
	float hrrn;
	t_list* recursos_asignados;
	int socket_consola;
	int tiempo_bloqueo;
	bool finalizar_proceso;
	char* motivo_exit;
}pcb_t;

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
    uint32_t pid;
}segmento_t;

typedef enum{
   	 EXITOSO,
   	 FALLIDO,
   	 COMPACTAR
} estados_segmentos;

typedef struct{ //A implmentar por FileSystem //todo fijarse que sea asi
	char* nombreArchivo;
	uint32_t puntero_directo;
	uint32_t puntero_indirecto;
	int tamanio_archivo;
	//... algo mas
	//t_queue* colaBloqueados; Lo pasamos a otra struct de kernel
	//pthread_mutex_t mutexArchivo; //para garantizar mutua exclusion
} fcb_t;


//enum para mandar las instrucciones
typedef enum{
	AX = 400,
	BX,
	CX,
	DX,
	EAX,
	EBX,
	ECX,
	EDX,
	RAX,
	RBX,
	RCX,
	RDX
}Registro;


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
	EXIT,
	HANDSHAKE,
	EXISTE_ARCHIVO,
	INICIAR_ESTRUCTURAS,
	FINALIZAR_ESTRUCTURAS,
	READ_CPU, //pa q memoria escriba lo que manda cpu y fs
	WRITE_CPU,
	READ,
	WRITE,
	READ_FS,
	WRITE_FS,
	ERROR,
	LEER_CPU,//segmentation fault
	SEGUI_EJECUTANDO // PARA VOS CPU
}op_code;

//
typedef enum{ //para que filesystem deveulva ok o no
	CORRECTO, //OK
	INCORRECTO,
	FINALIZAR //para que el filesystem avise a kernel cuando finaliza fread, fwirte, y ftruncate
}extra_code;






#endif /* SRC_ESTRUCTURAS_COMPARTIDAS_H_ */
