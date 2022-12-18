microshell: main.o fonctions.o
	gcc -o microshell main.o fonctions.o -g -O0

microshell.o: main.c
	gcc -c main.c -g -O0

fonctions.o: fonctions.h fonctions.c
	gcc -c fonctions.c -g -O0

clean:
	rm -f *.o

vclean: clean
	rm -f exemple
