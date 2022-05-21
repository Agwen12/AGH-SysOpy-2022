


all:
	@make -s clean
	@make -s serverC
	@make -s clientC

serverC:
	gcc -Wall -Wextra server.c -o server

clientC:
	gcc -Wall -Wextra client.c -o client


clean:
	rm -f server client