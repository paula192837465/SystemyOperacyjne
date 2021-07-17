CC = gcc -Wall -g 


static: main.c
	make clean
	$(CC) main.c -o main -lpthread -lm

run: static
	./main 4 numbers mona_lisa.ascii.pgm mona_lista_neg.ascii.pgm
	./main 10 blocks baboon.ascii.pgm baboon_neg.ascii.pgm
	./main 2 numbers saturn.ascii.pgm saturn_neg.ascii.pgm
	./main 8 numbers totem.ascii.pgm totem_neg.ascii.pgm
	./main 16 blocks x31_f18.ascii.pgm x31_f18_neg.ascii.pgm
	./main 1 numbers brain_508.ascii.pgm brain_neg.ascii.pgm


clean:
	rm -f *.o
	rm -f main