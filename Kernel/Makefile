# Globales
CC = gcc
OUTFILE = kernel.out

### Reglas ###

SRC_SHARED = $(shell find ../Shared/src -type f -name "*.c")
OBJ_SHARED = $(SRC_SHARED:.c=.o)

SRC_KERNEL = $(shell find ./src -type f -name "*.c")
OBJ_KERNEL = $(SRC_KERNEL:.c=.o)

CFLAGS = -I../Shared/src -c

build: makefolder $(OBJ_SHARED) $(OBJ_KERNEL)
	$(CC) -o $(OUTFILE) $(OBJ_SHARED) $(OBJ_KERNEL) -lpthread -lcommons -lm -lreadline

../Shared/src/%.o: ../Shared/src/%.c
	$(CC) $(CFLAGS) $< -o $@

./src/%.o: ./src/%.c
	$(CC) $(CFLAGS) $< -o $@

makefolder:
	if [ ! -d ./src ]; then mkdir src; fi
	
valgrind: build
	valgrind --leak-check=full --show-leak-kinds=all --log-file=valgrind.log ./$(OUTFILE)

clean:
	rm -rf src/*.o $(OUTFILE) valgrind.log