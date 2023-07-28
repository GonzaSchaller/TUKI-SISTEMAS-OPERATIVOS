#!/usr/bin/env bash
#Compilar e instalar las commons

#Compilar e instalar las shared
(cd Shared/Debug && make install)

#Compilar resto de los proyectos
(cd Kernel/ && make build) 
(cd Consola/ && make build) 
(cd Cpu/ && make build) 
(cd Memoria/ && make build) 
(cd File_System/ && make build)