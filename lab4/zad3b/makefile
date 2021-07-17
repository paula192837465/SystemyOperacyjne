CC = gcc -Wall -g 

run: catcher sender
	./catcher KILL & 
	./sender 30 `pidof catcher` KILL


clean:
	rm -f *.o
	rm -f catcher sender
