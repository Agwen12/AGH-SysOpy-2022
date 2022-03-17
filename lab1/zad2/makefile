
dir = ./text_files/
aloc7 =	allocate allocate  allocate  allocate  allocate  allocate  allocate
deloc0_6 = remove_block 0 remove_block 1 remove_block 2 remove_block 3 remove_block 4 remove_block 5 remove_block 6


quick:
	make clean
	gcc -Wall -c main.c
	gcc -Wall -c wclibrary.c
	ar rcs libwclibrary.a wclibrary.o
	gcc -Wall main.o libwclibrary.a -o main


static:
	gcc -Wall -c wclibrary.c
	ar rcs libwclibrary.a wclibrary.o

shared:
	gcc -Wall -c -fPIC  wclibrary.c
	gcc -shared -fPIC -o libwclibrary.so wclibrary.o



clean:
		rm -f *.o *.a *.so temp main raport2.txt

test: static
	gcc -Wall -c main.c
	gcc -Wall main.o libwclibrary.a -o main
	./main start_timer create_table 10 stop_timer "initialize array of size: 10"\
		   start_timer wc_command $(dir)small_1 stop_timer "counting 1 small file"\
		   start_timer allocate stop_timer "allocating small block"\
		   start_timer wc_command $(dir)small_1 $(dir)small_2 $(dir)small_3 stop_timer "counting 3 small files"\
		   start_timer allocate stop_timer "allocating slightly bigger block"\
		   start_timer wc_command $(dir)medium_1 stop_timer "counting 1 medium file"\
		   start_timer allocate stop_timer "allocating medium block"\
		   start_timer wc_command $(dir)big_1 stop_timer "counting 1 big file"\
		   start_timer allocate stop_timer "allocating big block"\
		   start_timer wc_command $(dir)small_1 $(dir)small_2 $(dir)small_3 $(dir)medium_1 $(dir)medium_2\
								  $(dir)medium_3 $(dir)big_1 $(dir)big_2 $(dir)big_3\
								  stop_timer "counting 9 files of various sizes"\
		   start_timer allocate stop_timer "allocating the biggest block"\
		   start_timer remove_block 0 stop_timer "deleting small block"\
		   start_timer remove_block 1 stop_timer "deleting slightly bigger block"\
		   start_timer remove_block 2 stop_timer "deleting medium block"\
		   start_timer remove_block 3 stop_timer "deleting big block"\
		   start_timer remove_block 4 stop_timer "deleting the biggest block"\
		   wc_command $(dir)small_1 $(dir)small_2 $(dir)small_3 $(dir)medium_1 $(dir)medium_2\
           								  $(dir)medium_3 $(dir)big_1 $(dir)big_2 $(dir)big_3\
           start_timer $(aloc7) $(deloc0_6) $(aloc7) $(deloc0_6) $(aloc7) $(deloc0_6) $(aloc7) $(deloc0_6)\
           stop_timer "adding and deleting 7 blocks"

