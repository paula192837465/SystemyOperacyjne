CC = gcc -Wall -o

clean:
	rm -f *.o
	rm -f *.a
	rm -f server
	rm -f client

compile:
	make clean
	$(CC) server server.c -lpthread -lm
	$(CC) client client.c -lpthread -lm

runserver:
	./server 8082 loc

runclient:
	./client
