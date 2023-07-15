#ifndef SRC_MANEJOS_H_
#define SRC_MANEJOS_H_
#include <stdio.h>
#include <stdlib.h>
#include <send_rcvs.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include "kernel.h"

void manejar_memoria(pcb_t*, uint32_t , uint32_t* );
void manejar_contextosDeEjecucion(pcb_t*, contexto_ejecucion contexto, t_list*);
void manejar_recursos(pcb_t*, uint32_t ,float , uint32_t*);
void manejar_otras_instrucciones(pcb_t*,uint32_t, float, uint32_t*);
void manejar_fileSystem(pcb_t* ,uint32_t, float, uint32_t*);
void eliminarArchivoDeTablaGlobal(char*, t_list*);
void eliminarArchivoDeTablaProceso(char*, t_list*);

void bloquear_procesoPorArchivo(void* );
void bloquear_procesoPorAIO(void* );
void recibir_actualizar_tablas_segmento(pcb_t*);

extern bool noSalePorIO;
recurso_sistema* encontrar_recurso(t_list*,char*);
fcb_kernel* encontrar_archivoTablaGlobal(t_list*, char*);
fcb_por_proceso* encontrar_archivoTablaProceso(t_list*, char*);

#endif /* SRC_MANEJOS_H_ */
