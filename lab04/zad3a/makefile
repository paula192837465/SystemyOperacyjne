CC = gcc -Wall -g 

# static: main.c
# 	make clean
	# $(CC) main.c -o main


run: catcher sender
	./catcher SIGRT & 
	./sender 30 `pidof catcher` SIGRT


clean:
	rm -f *.o
	rm -f catcher sender
