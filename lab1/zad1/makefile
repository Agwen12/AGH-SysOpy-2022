

static:
	gcc -Wall -c wclibrary.c
	ar rcs libwclibrary.a wclibrary.o

shared:
	gcc -Wall -c -fPIC  wclibrary.c
	gcc -shared -fPIC -o libwclibrary.so wclibrary.o

all: shared static

clean:
		rm -f *.o *.a *.so temp main