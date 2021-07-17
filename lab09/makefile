CC = gcc -Wall -g 


static: main.c
	make clean
	$(CC) main.c -o main -lpthread -lm

run: static
	./main


clean:
	rm -f *.o
	rm -f main