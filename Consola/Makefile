SRC_SHARED = $(shell find ../Shared/src -type f -name "*.c")
OBJ_SHARED = $(SRC_SHARED:.c=.o)

SRC_CONSOLA = $(shell find ./src -type f -name "*.c")
OBJ_CONSOLA = $(SRC_CONSOLA:.c=.o)

CFLAGS = -I../Shared/src -c

OUTFILE = consola.out

build: makefolder $(OBJ_SHARED) $(OBJ_CONSOLA)
	$(CC) -o $(OUTFILE) $(OBJ_SHARED) $(OBJ_CONSOLA) -lpthread -lcommons -lm -lreadline


makefolder:
	if [ ! -d ./src ]; then mkdir src; fi

valgrind: build
	valgrind --leak-check=full --show-leak-kinds=all --log-file=valgrind.log ./$(OUTFILE)

clean:
	rm -rf src/*.o $(OUTFILE) valgrind.log