
CC = gcc -Wall -g 


static: MyLibrary.c MyLibrary.h
	make clean
	$(CC) -c -I. MyLibrary.c
	ar rcs libMyLibrary.a MyLibrary.o

main_static: static main.c 
	$(CC) -c main.c
	$(CC) main.o -l MyLibrary -L ./ -o main


clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f main
	rm -f merge*.txt
	rm -f results.txt


t:
	make main_static
	make run

# run:
# 	./main create_table 2 merge_files A.txt:B.txt fileA.txt:fileB.txt 

run: 

	
	@echo "##################################################" 
	@echo "\n\n-----------------------Small files---------------------" 

	./main create_table 2 merge_files A.txt:B.txt fileA.txt:fileB.txt


	@echo "\n\n---------------------------Medium files----------------------------"
	./main create_table 1 merge_files PanTadeusz1.txt:PanTadeusz2.txt 
	./main create_table 2 merge_files praca1Medium.txt:praca2Medium.txt praca1Medium.txt:PanTadeusz1.txt
	./main create_table 1 merge_files PanTadeusz1.txt:zemsta1.txt 


	@echo "\n\n----------------------------Large files-------------------------" 
	./main create_table 1 merge_files PanTadeusz1Long.txt:PanTadeusz2Long.txt 
	./main create_table 3 merge_files longlong.txt:longlong2.txt PanTadeusz1Long.txt:PanTadeusz2Long.txt  merge_files longlong.txt:longlong2.txt
	./main create_table 4 merge_files longlong.txt:longlong2.txt PanTadeusz1Long.txt:PanTadeusz2Long.txt merge_files longlong.txt:longlong2.txt longlonglong1.txt:longlonglong2.txt
	
	

