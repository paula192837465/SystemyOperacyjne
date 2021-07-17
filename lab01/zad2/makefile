NAME =result2.txt
CC = gcc -Wall -g 


static: MyLibrary.c MyLibrary.h
	make clean
	$(CC) -c -I. MyLibrary.c
	ar rcs libMyLibrary.a MyLibrary.o

main_static: static main.c 
	$(CC) -c main.c
	$(CC) main.o -l MyLibrary -L ./ -o main

shared: MyLibrary.c MyLibrary.h
	make clean
	$(CC) -fPIC -c MyLibrary.c
	$(CC) -shared -fPIC -o libMyLibrary.so MyLibrary.o
	$(CC) main.c  -o main -L. -lMyLibrary -Wl,-rpath=`pwd`



clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f main
	rm -f merge.txt
	rm -f results.txt


t:
	make main_static
	make run

run: 

	@echo "##################################################" >> $(NAME)
	@echo "\n\n-----------------------Small files---------------------" >> $(NAME)

	./main create_table 2 merge_files A.txt:B.txt fileA.txt:fileB.txt >> $(NAME)
	./main create_table 3 merge_files A.txt:B.txt fileA.txt:fileB.txt remove_block 1 remove_row 0 3 merge_files fileA.txt:fileB.txt >> $(NAME)

	@echo "\n\n---------------------------Medium files----------------------------" >> $(NAME)
	./main create_table 1 merge_files PanTadeusz1.txt:PanTadeusz2.txt >> $(NAME)
	./main create_table 2 merge_files praca1Medium.txt:praca2Medium.txt praca1Medium.txt:PanTadeusz1.txt>> $(NAME)
	./main create_table 1 merge_files PanTadeusz1.txt:zemsta1.txt >> $(NAME)


	@echo "\n\n----------------------------Large files-------------------------" >> $(NAME)
	./main create_table 1 merge_files PanTadeusz1Long.txt:PanTadeusz2Long.txt >> $(NAME)
	./main create_table 3 merge_files longlong.txt:longlong2.txt PanTadeusz1Long.txt:PanTadeusz2Long.txt remove_block 1 merge_files longlong.txt:longlong2.txt>> $(NAME)
	./main create_table 4 merge_files longlong.txt:longlong2.txt PanTadeusz1Long.txt:PanTadeusz2Long.txt remove_block 1 merge_files longlong.txt:longlong2.txt longlonglong1.txt:longlonglong2.txt>> $(NAME)
