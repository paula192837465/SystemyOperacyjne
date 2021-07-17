CC = gcc -Wall -g 


static: main.c
	make clean
	$(CC) main.c -o main
	$(CC) producent.c -o producent
	$(CC) consument.c -o consument

run: static
	./main


clean:
	rm -f *.o
	rm -f main
