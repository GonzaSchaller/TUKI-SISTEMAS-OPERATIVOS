#include "planificacion.h"
pthread_mutex_t mutexNew;

void inicializarPCB(t_list* listaInstrucciones, pcb_t *pcb){

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

void agregarANew(pcb_t* pcb_proceso, t_log* log_kernel){
		pthread_mutex_lock(&mutexNew);

		queue_push(colaNew, pcb_proceso);
		log_info(log_kernel, "[NEW] Entra el proceso de PID: %d a la cola.", pcb_proceso->PID);

		pthread_mutex_unlock(&mutexNew);

		//log_trace(log_kernel,"Antes de analizar suspension en agregar a NEW ");

		//sem_post(&analizarSuspension); // Despierta al planificador de mediano plazo
		//log_trace(log_kernel,"Despues del post de analizar suspension en agregar a NEW ");

		//sem_wait(&suspensionFinalizada); // Espera a que ya se haya hecho, o no, la suspension

		//log_trace(log_kernel,"despues de suspension finalizada en agregar a NEW ");

		//sem_post(&contadorNew); // Despierta al planificador de largo plazo
		//sem_post(&largoPlazo);

		//log_error(log_kernel,"Salgo de agregar a NEW tranqui");
}
