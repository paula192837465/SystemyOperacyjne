CC = gcc -Wall -g 


static: main.c
	make clean
	$(CC) main.c -o main

run: static
	./main data


clean:
	rm -f *.o
	rm -f main
