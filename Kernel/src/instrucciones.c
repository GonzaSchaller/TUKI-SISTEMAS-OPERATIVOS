#include "instrucciones.h"

void cargar_instruccion(int id, char* nombre, uint32_t parametro1, uint32_t parametro2,uint32_t parametro3,t_list* lista){

	instruccion* estructura_instrucciones = malloc(sizeof(instruccion));

	estructura_instrucciones->id = id;
	estructura_instrucciones->nombre = nombre;
	estructura_instrucciones->parametro1 = parametro1;
	estructura_instrucciones->parametro2 = parametro2;
	estructura_instrucciones->parametro3 = parametro3;

	list_add(lista, estructura_instrucciones);
}

