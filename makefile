################################################################################
CC=gcc
CFLAGS=-g -Wall -pedantic -ansi -std=c99
EJS=practica2

################################################################################
PRACTICA2= minimiza.o main.o lib_afnd_64.a

################################################################################
all: $(EJS)

practica2: $(PRACTICA2)
	$(CC) $(CFLAGS) -g -o practica2 $(PRACTICA2)


transforma.o:minimiza.c minimiza.h minimiza.h
	$(CC) $(CFLAGS) -g -c minimiza.c


main.o:main.c afnd.h minimiza.h
	$(CC) -c -o  $@ $< $(CFLAGS)

################################################################################

val:
	valgrind --leak-check=yes --tool=memcheck ./practica2

exe:
	./practica2

clean:
	rm -rf *.o $(EJS)

foto:
	dot -Tpng af1.dot -o minimiza.png
