
all: emit scan parse

fruit.o: fruit.c fruit.h
	gcc -c -g -O0 -Wall fruit.c

emit.o: emit.c fruit.h
	gcc -c -g -O0 -Wall emit.c

emit: fruit.o emit.o
	gcc -o emit fruit.o emit.o -lyaml

scan.o: scan.c
	gcc -c -g -O0 -Wall scan.c

scan: scan.o
	gcc -o scan scan.o -lyaml

parse.o: parse.c fruit.h
	gcc -c -g -O0 -Wall parse.c

parse: fruit.o parse.o
	gcc -o parse fruit.o parse.o -lyaml

clean:
	rm -f emit scan parse
	rm -f *.o core
