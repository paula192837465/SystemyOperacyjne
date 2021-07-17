CC = gcc -Wall -g 


static: main.c
	make clean
	$(CC) main.c -o main -lm

run: static
	./main dane.txt


clean:
	rm -f *.o
	rm -f main


	

