


quick:
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
		rm -f *.o *.a *.so temp main