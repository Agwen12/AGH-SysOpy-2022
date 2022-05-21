CC = gcc
CFLAGS = -Wall -g

all: test

compile:
	$(CC) $(FLAGS) main.c -o main
	$(CC) $(FLAGS) consumer.c -o consumer
	$(CC) $(FLAGS) producer.c -o producer

test: clean compile
	./main

clean:
	rm -f main consumer producer pipe

clean_res:
	rm -f ./consumer_files/*