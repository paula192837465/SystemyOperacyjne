CC = gcc -Wall -g 


static: main.c
	make clean
	$(CC) main.c -o main
	$(CC) cook.c -o cook
	$(CC) supplier.c -o supplier

run: static
	./main 5 5

clean:
	rm -f *.o
	rm -f main