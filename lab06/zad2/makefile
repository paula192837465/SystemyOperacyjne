CC = gcc -Wall -o

clean:
	rm -f *.o
	rm -f *.a
	rm -f server
	rm -f client
	

compile:
	make clean
	$(CC) server server.c -lrt
	$(CC) client client.c -lrt

runserver:
	./server

runclient:
	./client

