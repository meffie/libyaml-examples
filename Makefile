
all: emit scan parse

emit.o: emit.c
	gcc -c emit.c -g -O0 -Wall

emit: emit.o
	gcc -o emit emit.o -lyaml

scan.o: scan.c
	gcc -c scan.c -g -O0 -Wall

scan: scan.o
	gcc -o scan scan.o -lyaml

parse.o: parse.c
	gcc -c parse.c -g -O0 -Wall

parse: parse.o
	gcc -o parse parse.o -lyaml

clean:
	rm -f emit scan parse
	rm -f *.o core
