CC = gcc
CFLAGS = -Wall -g

all: test

compile:
	$(CC) $(FLAGS) main.c -o main
	$(CC) $(FLAGS) consumer.c -o consumer
	$(CC) $(FLAGS) producer.c -o producer

test: clean compile
	./main
#	cat consumer_files/many_to_many_res.txt
#	printf "\n"
#	cat consumer_files/many_to_one_res.txt
#	printf "\n"
#	cat consumer_files/one_to_many_res.txt

clean:
	rm -f main consumer producer pipe

clean_res:
	rm -f ./consumer_files/*