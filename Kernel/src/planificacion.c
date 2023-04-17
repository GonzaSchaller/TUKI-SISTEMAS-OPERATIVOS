#include <planificacion.h>
pthread_mutex_t mutexNew;

void inicializarPCB(t_list* listaInstrucciones,pcb_t *pcb){

	pcb->PID = contadorProcesos;
	pcb->instrucciones = listaInstrucciones;
	pcb->PC = 0;
	//pcb->regCpu;
	//pcb->TSegmento;
	//pcb->estimacionRafaga;
	//pcb->tiempReady;
	//pcb->tablaArchivos;
	pcb->state= NEW;

}

void agregarANew(pcb_t* pcb){

}
