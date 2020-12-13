################################################################################
CC=gcc
CFLAGS=-g -Wall -pedantic -ansi -std=c99
EJS=practica2 practica2abc

################################################################################
PRACTICA2= minimiza.o main.o lib_afnd_64.a
PRACTICA2ABC= minimiza.o main2.o lib_afnd_64.a

################################################################################
all: $(EJS)

practica2: $(PRACTICA2)
	$(CC) $(CFLAGS) -g -o practica2 $(PRACTICA2)

practica2abc: $(PRACTICA2ABC)
	$(CC) $(CFLAGS) -g -o practica2abc $(PRACTICA2ABC)


transforma.o:minimiza.c minimiza.h minimiza.h
	$(CC) $(CFLAGS) -g -c minimiza.c


main.o:main.c afnd.h minimiza.h
	$(CC) -c -o  $@ $< $(CFLAGS)

main2.o:main2.c afnd.h minimiza.h
	$(CC) -c -o  $@ $< $(CFLAGS)

################################################################################

val:
	valgrind --leak-check=yes --tool=memcheck ./practica2

val2:
	valgrind --leak-check=yes --tool=memcheck ./practica2abc

exe:
	./practica2

exe2:
	./practica2abc

clean:
	rm -rf *.o $(EJS) afd1.txt afd2.txt

foto:
	dot -Tpng af1.dot -o minimiza.png
