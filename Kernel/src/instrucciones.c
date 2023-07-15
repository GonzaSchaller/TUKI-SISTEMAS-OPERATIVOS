#include "instrucciones.h"

//IMPORTANTE: HABRIA QUE HACAERLA GENERICA O INGENIARNOSLA PARA QUE ANDE TOD0
//carga la lista de instrucciones para el pcb

//tipo int int int
void cargar_instruccion1(int id, char* nombre, uint32_t parametro1, uint32_t parametro2,uint32_t parametro3,t_list* lista){

	instruccion* estructura_instrucciones = malloc(sizeof(instruccion));

	estructura_instrucciones->id = id;
	estructura_instrucciones->nombre = nombre;
	estructura_instrucciones->parametro1.tipo_int = parametro1;
	estructura_instrucciones->parametro2.tipo_int = parametro2;
	estructura_instrucciones->parametro3 = parametro3;

	list_add(lista, estructura_instrucciones);
}
//Tipo int char* int
void cargar_instruccion2(int id, char* nombre, uint32_t parametro1, char* parametro2,uint32_t parametro3,t_list* lista){

	instruccion* estructura_instrucciones = malloc(sizeof(instruccion));

	estructura_instrucciones->id = id;
	estructura_instrucciones->nombre = nombre;
	estructura_instrucciones->parametro1.tipo_int = parametro1;
	estructura_instrucciones->parametro2.tipo_string= parametro2;
	estructura_instrucciones->parametro3 = parametro3;

	list_add(lista, estructura_instrucciones);
}
//Tipo char* int int
void cargar_instruccion3(int id, char* nombre, char* parametro1, uint32_t parametro2,uint32_t parametro3,t_list* lista){

	instruccion* estructura_instrucciones = malloc(sizeof(instruccion));

	estructura_instrucciones->id = id;
	estructura_instrucciones->nombre = nombre;
	estructura_instrucciones->parametro1.tipo_string = parametro1;
	estructura_instrucciones->parametro2.tipo_int = parametro2;
	estructura_instrucciones->parametro3 = parametro3;

	list_add(lista, estructura_instrucciones);
}
void asignar_recurso(char* nombre,t_list* lista){

	recurso_sistema* recurso = malloc(sizeof(recurso_sistema));

	recurso->nombre = strdup(nombre);
	recurso->instancia = 1;

	list_add(lista, recurso);
}
void aumentar_instancias_recurso( char* nombre,t_list* lista){

	recurso_sistema* recurso = encontrar_recurso(lista,nombre);
	recurso->instancia += 1;
}

void inicializarPCB(int contadorProceso, t_list* listaInstrucciones, pcb_t *pcb,int socket_cliente){
	pcb->contexto.registros.AX[sizeof(pcb->contexto.registros.AX) - 1] = '\0';
	pcb->contexto.registros.BX[sizeof(pcb->contexto.registros.BX) - 1] = '\0';
	pcb->contexto.registros.CX[sizeof(pcb->contexto.registros.CX) - 1] = '\0';
	pcb->contexto.registros.DX[sizeof(pcb->contexto.registros.DX) - 1] = '\0';
	pcb->contexto.registros.EAX[sizeof(pcb->contexto.registros.EAX) - 1] = '\0';
	pcb->contexto.registros.EBX[sizeof(pcb->contexto.registros.EBX) - 1] = '\0';
	pcb->contexto.registros.ECX[sizeof(pcb->contexto.registros.ECX) - 1] = '\0';
	pcb->contexto.registros.EDX[sizeof(pcb->contexto.registros.EDX) - 1] = '\0';
	pcb->contexto.registros.RAX[sizeof(pcb->contexto.registros.RAX) - 1] = '\0';
	pcb->contexto.registros.RBX[sizeof(pcb->contexto.registros.RBX) - 1] = '\0';
	pcb->contexto.registros.RCX[sizeof(pcb->contexto.registros.RCX) - 1] = '\0';
	pcb->contexto.registros.RDX[sizeof(pcb->contexto.registros.RDX) - 1] = '\0';
	//pcb->instrucciones = list_create();
    pcb->contexto.PID = contadorProceso;
    pcb->instrucciones = list_create(); // Crear una nueva lista vacía

        // Copiar elementos de la lista pasada como parámetro a la lista del PCB
        for (int i = 0; i < list_size(listaInstrucciones); i++) {
            void* instruccion = list_get(listaInstrucciones, i);
            list_add(pcb->instrucciones, instruccion);
        }
    pcb->contexto.PC = 0;
    t_list* lista = list_create();
   	//pcb->contexto.registros = *registrosInicializados;
    pcb->contexto.TSegmento= lista;
    pcb->horaDeIngresoAReady= 0;
    pcb->tabla_archivosAbiertos = list_create();
    pcb->recursos_asignados = list_create();
    pcb->state= NEW;
    pcb->tiempo_bloqueo = 0;
    pcb->state_anterior= 0;
    pcb->estimacion_rafaga_anterior = estimacion_inicial;
	pcb-> rafaga_anterior_real = 0;
	pcb->horaDeIngresoAExe = 0;
	pcb->estimacion_prox_rafaga = (hrrn_alfa* pcb->rafaga_anterior_real)+ ((1-hrrn_alfa)* pcb-> estimacion_rafaga_anterior);
	pcb->socket_consola = socket_cliente;
	pcb->tiempo_bloqueo = -1;
	pcb->finalizar_proceso=false;
	//free(&listaInstrucciones);
}

void enviar_pcb_cpu(int fd_cpu, pcb_t* pcb_proceso){
	send_CANT_INSTRUCCIONES(fd_cpu, list_size(pcb_proceso->instrucciones));
	send_instrucciones_kernel_a_cpu(fd_cpu,pcb_proceso);
	send_CONTEXTO_EJECUCION(fd_cpu,pcb_proceso->contexto);
	send_TABLA_SEGMENTOS(fd_cpu, pcb_proceso->contexto.TSegmento);
}

void send_instrucciones_kernel_a_cpu(int fd_cpu,pcb_t* pcb_proceso){
		instruccion* a;
		int cant_instrucciones = list_size(pcb_proceso->instrucciones);
		int indice = 0;
		while(indice < cant_instrucciones){
			a = list_get(pcb_proceso->instrucciones,indice);

			if(a->id == SET){

				send_SET(fd_cpu,a->parametro1.tipo_int,a->parametro2.tipo_string);

			}
			else if(a->id == MOV_IN){

				send_MOV_IN(fd_cpu,a->parametro1.tipo_int, a->parametro2.tipo_int);

			}

			else if(a->id == MOV_OUT){

				send_MOV_OUT(fd_cpu,a->parametro1.tipo_int,a-> parametro2.tipo_int);
			}
			else if(a->id == IO){

				send_IO(fd_cpu,a->parametro1.tipo_int);
			}
			else if(a->id == F_OPEN){

				send_F_OPEN(fd_cpu,a->parametro1.tipo_string);

			}
			else if(a->id == F_CLOSE){

				send_F_CLOSE(fd_cpu,a->parametro1.tipo_string);
			}

			else if(a->id == F_SEEK){

				send_F_SEEK(fd_cpu,a->parametro1.tipo_string, a->parametro2.tipo_int);
			}

			else if(a->id == F_READ){

				send_F_READ(fd_cpu,a->parametro1.tipo_string, a->parametro2.tipo_int,a->parametro3);
			}

			else if(a->id == F_WRITE ){

				send_F_WRITE (fd_cpu,a->parametro1.tipo_string, a->parametro2.tipo_int,a->parametro3);
			}

			else if(a->id == F_TRUNCATE ){

				send_F_TRUNCATE(fd_cpu,a->parametro1.tipo_string, a->parametro2.tipo_int);
			}

			else if(a->id == WAIT ){

				send_WAIT (fd_cpu,a->parametro1.tipo_string);
			}

			else if(a->id == SIGNAL ){

				send_SIGNAL (fd_cpu,a->parametro1.tipo_string);
			}

			else if(a->id == CREATE_SEGMENT ){

				send_CREATE_SEGMENT (fd_cpu,a->parametro1.tipo_int, a->parametro2.tipo_int);
			}

			else if(a->id == DELETE_SEGMENT ){

				send_DELETE_SEGMENT (fd_cpu,a->parametro1.tipo_int);
			}

			else if(a->id == YIELD) {

				send_YIELD(fd_cpu);
			}

			else if(a->id == EXIT || strcmp(a->nombre, "EXIT") == 0){

				send_EXIT(fd_cpu);
			}
			indice++;
		}

}




