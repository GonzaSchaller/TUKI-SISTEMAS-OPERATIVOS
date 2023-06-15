#include "funciones.h"

int obtener_registro_cpu(char* registro) {
    Registro reg;

    if (strcmp(registro, "AX") == 0) {
        reg = AX;
    } else if (strcmp(registro, "BX") == 0) {
        reg = BX;
    } else if (strcmp(registro, "CX") == 0) {
        reg = CX;
    } else if (strcmp(registro, "DX") == 0) {
        reg = DX;
    } else if (strcmp(registro, "EAX") == 0) {
        reg = EAX;
    } else if (strcmp(registro, "EBX") == 0) {
        reg = EBX;
    } else if (strcmp(registro, "ECX") == 0) {
        reg = ECX;
    } else if (strcmp(registro, "EDX") == 0) {
        reg = EDX;
    } else if (strcmp(registro, "RAX") == 0) {
        reg = RAX;
    } else if (strcmp(registro, "RBX") == 0) {
        reg = RBX;
    } else if (strcmp(registro, "RCX") == 0) {
        reg = RCX;
    } else if (strcmp(registro, "RDX") == 0) {
        reg = RDX;}

    return reg;
}

 void obtener_instrucciones(char*path){

    FILE*archivo = fopen(path,"r");
    if(archivo == NULL) log_error(log_consola,"Error abriendo archivo");

    char buffer[100];
    char*token;

    while(fgets(buffer, 100, archivo)){ //leo linea por linea, de a 99 caracteres + /0
    	token = strtok(buffer,"\n");
    	instruccion* estructura_instrucciones = malloc(sizeof(instruccion));

        //3 parametros
        if(strncmp(buffer,"F_READ",6) == 0){


    		char** parametros = string_n_split(token,4," ");

    		estructura_instrucciones->id = F_READ;
    		estructura_instrucciones->nombre = parametros[0];
    		estructura_instrucciones->parametro1.tipo_string = parametros[1];
    		estructura_instrucciones->parametro2.tipo_int = atoi(parametros[2]);
    		estructura_instrucciones->parametro3 = atoi(parametros[3]);

    		list_add(lista_instrucciones,estructura_instrucciones);
        }

        else if(strncmp(buffer,"F_WRITE",7) == 0){

    		char** parametros = string_n_split(token,4," ");

    		estructura_instrucciones->id = F_WRITE;
    		estructura_instrucciones->nombre = parametros[0];
    		estructura_instrucciones->parametro1.tipo_string = parametros[1];
    		estructura_instrucciones->parametro2.tipo_int = atoi(parametros[2]);
    		estructura_instrucciones->parametro3 = atoi(parametros[3]);

    		list_add(lista_instrucciones,estructura_instrucciones);

        }
        //2 parametros

        else if(strncmp(buffer,"SET",3) == 0){

    		char** parametros = string_n_split(token, 3," ");

    		estructura_instrucciones->id = SET;
    		estructura_instrucciones->nombre = parametros[0];
    		estructura_instrucciones->parametro1.tipo_int = obtener_registro_cpu(parametros[1]);
    		estructura_instrucciones->parametro2.tipo_string = parametros[2];
    		//estructura_instrucciones->parametro3 = NULL;

    		list_add(lista_instrucciones,estructura_instrucciones);


               }
        else if(strncmp(buffer,"MOV_IN",6) == 0){
    		char** parametros = string_n_split(token,3," ");

    		estructura_instrucciones->id = MOV_IN;
    		estructura_instrucciones->nombre = parametros[0];

    		estructura_instrucciones->parametro1.tipo_int = obtener_registro_cpu(parametros[1]);
    		estructura_instrucciones->parametro2.tipo_int= atoi(parametros[2]);
    		//estructura_instrucciones->parametro3 = NULL;

    		list_add(lista_instrucciones,estructura_instrucciones);


               }
        else if(strncmp(buffer,"MOV_OUT", 7) == 0){
    		char** parametros = string_n_split(token,3," ");

    		estructura_instrucciones->id = MOV_OUT;
    		estructura_instrucciones->nombre = parametros[0];
    		estructura_instrucciones->parametro1.tipo_int = atoi(parametros[1]);
    		estructura_instrucciones->parametro2.tipo_int = obtener_registro_cpu(parametros[2]);
    		//estructura_instrucciones->parametro3 = NULL;

    		list_add(lista_instrucciones,estructura_instrucciones);

               }
        else if(strncmp(buffer,"F_TRUNCATE", 10) == 0){
    		char** parametros = string_n_split(token,3," ");

    		estructura_instrucciones->id = F_TRUNCATE;
    		estructura_instrucciones->nombre = parametros[0];
    		estructura_instrucciones->parametro1.tipo_string = parametros[1];
    		estructura_instrucciones->parametro2.tipo_int = atoi(parametros[2]);
    		//estructura_instrucciones->parametro3 = NULL;

    		list_add(lista_instrucciones,estructura_instrucciones);

               }
        else if(strncmp(buffer,"F_SEEK",6) == 0){
    		char** parametros = string_n_split(token,3," ");

    		estructura_instrucciones->id = F_SEEK;
    		estructura_instrucciones->nombre = parametros[0];
    		estructura_instrucciones->parametro1.tipo_string = parametros[1];
    		estructura_instrucciones->parametro2.tipo_int = atoi(parametros[2]);
    		//estructura_instrucciones->parametro3 = NULL;

    		list_add(lista_instrucciones,estructura_instrucciones);

               }

        else if(strncmp(buffer,"CREATE_SEGMENT", 14) == 0){
    		char** parametros = string_n_split(token,3," ");

    		estructura_instrucciones->id = CREATE_SEGMENT;
    		estructura_instrucciones->nombre = parametros[0];
    		estructura_instrucciones->parametro1.tipo_int =atoi(parametros[1]);
    		estructura_instrucciones->parametro2.tipo_int = atoi(parametros[2]);
    		//estructura_instrucciones->parametro3 = NULL;

    		list_add(lista_instrucciones,estructura_instrucciones);

               }
        // 1 parametro
        else if(strncmp(buffer,"I/O", 3) == 0){
    		char** parametros = string_n_split(token,2," ");

    		estructura_instrucciones->id = IO;
    		estructura_instrucciones->nombre = parametros[0];
    		estructura_instrucciones->parametro1.tipo_int = atoi(parametros[1]);
    		//estructura_instrucciones->parametro2.tipo_int = NULL;
    		//estructura_instrucciones->parametro3 = NULL;

    		list_add(lista_instrucciones,estructura_instrucciones);

               }
        else if(strncmp(buffer,"WAIT", 4) == 0){
    		char** parametros = string_n_split(token,2," ");

    		estructura_instrucciones->id = WAIT;
    		estructura_instrucciones->nombre = parametros[0];
    		estructura_instrucciones->parametro1.tipo_string = parametros[1];
    		//estructura_instrucciones->parametro2.tipo_int = NULL;
    		//estructura_instrucciones->parametro3 = NULL;

    		list_add(lista_instrucciones,estructura_instrucciones);

               }
        else if(strncmp(buffer,"SIGNAL", 6) == 0){
    		char** parametros = string_n_split(token,2," ");

    		estructura_instrucciones->id = SIGNAL;
    		estructura_instrucciones->nombre = parametros[0];
    		estructura_instrucciones->parametro1.tipo_string = parametros[1];
    		//estructura_instrucciones->parametro2.tipo_int = NULL;
    		//estructura_instrucciones->parametro3 = NULL;

    		list_add(lista_instrucciones,estructura_instrucciones);

               }
        else if(strncmp(buffer,"F_OPEN", 6) == 0){
    		char** parametros = string_n_split(token,2," ");

    		estructura_instrucciones->id = F_OPEN;
    		estructura_instrucciones->nombre = parametros[0];
    		estructura_instrucciones->parametro1.tipo_string = parametros[1];
    		//estructura_instrucciones->parametro2.tipo_int = NULL;
    		//estructura_instrucciones->parametro3 = NULL;

    		list_add(lista_instrucciones,estructura_instrucciones);

               }
        else if(strncmp(buffer,"F_CLOSE", 7) == 0){
    		char** parametros = string_n_split(token,2," ");

    		estructura_instrucciones->id = F_CLOSE;
    		estructura_instrucciones->nombre = parametros[0];
    		estructura_instrucciones->parametro1.tipo_string = parametros[1];
    		//estructura_instrucciones->parametro2.tipo_int = NULL;
    		//estructura_instrucciones->parametro3 = NULL;

    		list_add(lista_instrucciones,estructura_instrucciones);

               }
        else if(strncmp(buffer,"DELETE_SEGMENT",14) == 0){
    		char** parametros = string_n_split(token,2," ");

    		estructura_instrucciones->id = DELETE_SEGMENT;
    		estructura_instrucciones->nombre = parametros[0];
    		estructura_instrucciones->parametro1.tipo_int = atoi(parametros[1]);
    		//estructura_instrucciones->parametro2.tipo_int = NULL;
    		//estructura_instrucciones->parametro3 = NULL;
    		list_add(lista_instrucciones,estructura_instrucciones);

               }
        //0 parametros
        else if(strncmp(buffer,"EXIT", 4) == 0){

    		char** parametros = string_n_split(token,1," ");

    		estructura_instrucciones->id = EXIT;
    		estructura_instrucciones->nombre = parametros[0];
    		//estructura_instrucciones->parametro1.tipo_int = NULL;
    		//estructura_instrucciones->parametro2.tipo_int= NULL;
    		//estructura_instrucciones->parametro3 = NULL;

    		list_add(lista_instrucciones,estructura_instrucciones);

               }
        else if(strncmp(buffer,"YIELD", 5) == 0){

    		char** parametros = string_n_split(token,1," ");

    		estructura_instrucciones->id = YIELD;
    		estructura_instrucciones->nombre = parametros[0];
    		//estructura_instrucciones->parametro1.tipo_int = NULL;
    		//estructura_instrucciones->parametro2.tipo_int = NULL;
    		//estructura_instrucciones->parametro3 = NULL;

    		list_add(lista_instrucciones,estructura_instrucciones);
               }

     }

   fclose(archivo);
}

void send_instrucciones_a_kernel(){
	instruccion* a ;
	int cant_instrucciones = list_size(lista_instrucciones);
	int indice = 0;

	//log_error(log_consola,"El socket dentro de send instrcciones es : %d",fd_mod2);
	//log_error(log_consola,"El tam recibido por parametro a enviar es: %d",tam);

	while(indice < cant_instrucciones){
		a = list_get(lista_instrucciones,indice);

		//log_warning(log_consola,"El id de las intrucciones en send instrucciones es: %d",a->id);

					if(a->id == SET){

						send_SET(conexion_kernel,a->parametro1.tipo_int,a->parametro2.tipo_string);

					}
					else if(a->id == MOV_IN){

						send_MOV_IN(conexion_kernel,a->parametro1.tipo_int, a->parametro2.tipo_int);

					}

					else if(a->id == MOV_OUT){

						send_MOV_OUT(conexion_kernel,a->parametro1.tipo_int,a-> parametro2.tipo_int);
					}
					else if(a->id == IO){

						send_IO(conexion_kernel,a->parametro1.tipo_int);
					}
					else if(a->id == F_OPEN){

						send_F_OPEN(conexion_kernel,a->parametro1.tipo_string);

					}
					else if(a->id == F_CLOSE){

						send_F_CLOSE(conexion_kernel,a->parametro1.tipo_string);
					}

					else if(a->id == F_SEEK){

						send_F_SEEK(conexion_kernel,a->parametro1.tipo_string, a->parametro2.tipo_int);
					}

					else if(a->id == F_READ){

						send_F_READ(conexion_kernel,a->parametro1.tipo_string, a->parametro2.tipo_int,a->parametro3);
					}

					else if(a->id == F_WRITE ){

						send_F_WRITE (conexion_kernel,a->parametro1.tipo_string, a->parametro2.tipo_int,a->parametro3);
					}

					else if(a->id == F_TRUNCATE ){

						send_F_TRUNCATE(conexion_kernel,a->parametro1.tipo_string, a->parametro2.tipo_int);
					}

					else if(a->id == WAIT ){

						send_WAIT (conexion_kernel,a->parametro1.tipo_string);
					}

					else if(a->id == SIGNAL ){

						send_SIGNAL (conexion_kernel,a->parametro1.tipo_string);
					}

					else if(a->id == CREATE_SEGMENT ){

						send_CREATE_SEGMENT (conexion_kernel,a->parametro1.tipo_int, a->parametro2.tipo_int);
					}

					else if(a->id == DELETE_SEGMENT ){

						send_DELETE_SEGMENT (conexion_kernel,a->parametro1.tipo_int);
					}

					else if(a->id == YIELD) {

						send_YIELD(conexion_kernel);
					}

					else if(a->id == EXIT || strcmp(a->nombre, "EXIT") == 0){

						send_EXIT(conexion_kernel);
					}
					indice++;
				}
		send_CANT_INSTRUCCIONES(conexion_kernel, 153); //todo revisar numero tosquedad para frenar el rcv consola de kernel
		list_destroy_and_destroy_elements(lista_instrucciones,free);
}


