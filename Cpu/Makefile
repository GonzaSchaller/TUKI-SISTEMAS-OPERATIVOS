SRC_SHARED = $(shell find ../Shared/src -type f -name "*.c")
OBJ_SHARED = $(SRC_SHARED:.c=.o)

SRC_CPU = $(shell find ./src -type f -name "*.c")
OBJ_CPU = $(SRC_CPU:.c=.o)

CFLAGS = -I../Shared/src -c

OUTFILE = cpu.out

build: makefolder $(OBJ_SHARED) $(OBJ_CPU)
	$(CC) -o $(OUTFILE) $(OBJ_SHARED) $(OBJ_CPU) -lpthread -lcommons -lm -lreadline

makefolder:
	if [ ! -d ./src ]; then mkdir src; fi

valgrind: build
	valgrind --leak-check=full --show-leak-kinds=all --log-file=valgrind.log ./$(OUTFILE)

clean:
	rm -rf src/*.o $(OUTFILE) valgrind.log
