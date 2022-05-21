
CC = gcc
CFLAGS = -Wall -g

all: run

compile:
	@$(CC) $(FLAGS) chef.c -o chef -lpthread -lrt
	@$(CC) $(FLAGS) uberEats.c -o uberEats -lpthread -lrt
	@$(CC) $(FLAGS) main.c -o main -lpthread -lrt

run: compile
	./main $(ARGS)


clean:
	@rm -f main chef uberEats oven table