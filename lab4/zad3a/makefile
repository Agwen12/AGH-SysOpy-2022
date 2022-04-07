



comp:
	gcc -Wall sender.c -o sender
	gcc -Wall catcher.c -o catcher


clean:
	rm -f sender catcher


kill-s:
	kill -9 `pgrep sender`

kill-c:
	kill -9 `pgrep catcher`

kill-all: kill-c kill-s


tes:
	./sender `pgrep catcher` 1000 $(MODE)
