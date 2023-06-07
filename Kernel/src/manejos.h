#ifndef SRC_MANEJOS_H_
#define SRC_MANEJOS_H_
#include <stdio.h>
#include <stdlib.h>
#include <send_rcvs.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include "kernel.h"

void manejar_memoria(pcb_t*, uint32_t );
void manejar_contextosDeEjecucion(pcb_t*);
void manejar_recursos(pcb_t*, uint32_t ,float );
void manejar_otras_instrucciones(pcb_t*,uint32_t, float);
void manejar_fileSystem(pcb_t* ,uint32_t, float);
void eliminarArchivoDeLista(char*, t_list*);
void bloquear_procesoPorArchivo(void* );
void bloquear_procesoPorAIO(void* );

recurso_sistema* encontrar_recurso(t_list*,char*);
fcb_t* encontrar_archivo(t_list*, char*);

#endif /* SRC_MANEJOS_H_ */
