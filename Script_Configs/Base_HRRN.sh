#Creo los archivos de configuracion de MEMORIA

cd ..
cd Memoria/

FILE=/home/utnso/tp-2023-1c-Kernel-Masters/memoria.config
if test -f "$FILE"; then
    rm memoria.config
fi

touch memoria.config

echo 'PUERTO_ESCUCHA=8002' >memoria.config
echo 'TAM_MEMORIA=4096' >>memoria.config
echo 'TAM_SEGMENTO_0=128' >>memoria.config
echo 'CANT_SEGMENTOS=16' >>memoria.config
echo 'RETARDO_MEMORIA=1000' >>memoria.config
echo 'RETARDO_COMPACTACION=60000' >>memoria.config
echo 'ALGORITMO_ASIGNACION=BEST' >>memoria.config

cd .. 

#Creo los archivos de configuracion de KERNEL

cd Kernel/

FILE=/home/utnso/tp-2023-1c-Kernel-Masters/kernel.config
if test -f "$FILE"; then
    rm kernel.config
fi

touch kernel.config

echo 'IP_MEMORIA=127.0.0.1' >kernel.config
echo 'PUERTO_MEMORIA=8002' >>kernel.config
echo 'IP_FILESYSTEM=127.0.0.1' >>kernel.config
echo 'PUERTO_FILESYSTEM=8003' >>kernel.config
echo 'IP_CPU=127.0.0.1' >>kernel.config
echo 'PUERTO_CPU=8001' >>kernel.config
echo 'PUERTO_ESCUCHA=8000' >>kernel.config
echo 'ALGORITMO_PLANIFICACION=HRRN' >>kernel.config
echo 'ESTIMACION_INICIAL=2000' >>kernel.config
echo 'HRRN_ALFA=0.5' >>kernel.config
echo 'GRADO_MAX_MULTIPROGRAMACION=4' >>kernel.config
echo 'RECURSOS=[DISCO]' >>kernel.config
echo 'INSTANCIAS_RECURSOS=[1]' >>kernel.config

cd .. 

#Creo los archivos de configuracion de CPU

cd Cpu/

FILE=/home/utnso/tp-2023-1c-Kernel-Masters/cpu.config
if test -f "$FILE"; then
    rm cpu.config
fi

touch cpu.config

echo 'RETARDO_INSTRUCCION=1000' >cpu.config
echo 'IP_MEMORIA=127.0.0.1' >>cpu.config
echo 'PUERTO_MEMORIA=8002' >>cpu.config
echo 'PUERTO_ESCUCHA=8001' >>cpu.config
echo 'TAM_MAX_SEGMENTO=128' >>cpu.config

cd ..

cd File_System

FILE=/home/utnso/tp-2023-1c-Kernel-Masters/fileSystem.config
if test -f "$FILE"; then
    rm fileSystem.config
fi

touch fileSystem.config
echo 'IP_MEMORIA=127.0.0.1' >fileSystem.config
echo 'PUERTO_MEMORIA=8002' >>fileSystem.config
echo 'PUERTO_ESCUCHA=8003' >>fileSystem.config
echo 'RETARDO_ACCESO_BLOQUE=15000' >>fileSystem.config
echo 'PATH_SUPERBLOQUE=/home/utnso/tp-2023-1c-Kernel-Masters/File_System/superbloque.dat' >>fileSystem.config
echo 'PATH_BITMAP=/home/utnso/tp-2023-1c-Kernel-Masters/File_System/bitmap.dat' >>fileSystem.config
echo 'PATH_BLOQUES=/home/utnso/tp-2023-1c-Kernel-Masters/File_System/bloques.dat' >>fileSystem.config
echo 'PATH_FCB=/home/utnso/tp-2023-1c-Kernel-Masters/File_System/fcb' >>fileSystem.config

