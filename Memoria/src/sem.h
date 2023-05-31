#ifndef SEM_H
#define SEM_H


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>
#include <sockets.h>
#include <pthread.h>

void iniciar_mutex();
void finalizar_mutex();

#endif
