SRC_SHARED = $(shell find ../Shared/src -type f -name "*.c")
OBJ_SHARED = $(SRC_SHARED:.c=.o)

SRC_MEMORIA = $(shell find ./src -type f -name "*.c")
OBJ_MEMORIA = $(SRC_MEMORIA:.c=.o)

CFLAGS = -I../Shared/src -c

OUTFILE = memoria.out

build: makefolder $(OBJ_SHARED) $(OBJ_MEMORIA)
	$(CC) -o $(OUTFILE) $(OBJ_SHARED) $(OBJ_MEMORIA) -lpthread -lcommons -lm

makefolder:
	if [ ! -d ./src ]; then mkdir src; fi

valgrind: build
	valgrind --leak-check=full --show-leak-kinds=all --log-file=valgrind.log ./$(OUTFILE)

clean:
	rm -rf src/*.o $(OUTFILE) valgrind.log
