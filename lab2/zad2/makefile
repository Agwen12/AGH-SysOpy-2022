lib:
	gcc -Wall main.c -o zad2 -D LIB

sys:
	gcc -Wall main.c -o zad2 -D SYS

tests:
	make lib
	echo "##### LIB #####" >> pomiar_zad_2.txt
	echo "10kB file:" >> pomiar_zad_2.txt
	time -p ./zad2 a test 2>> pomiar_zad_2.txt
	echo "100kB file:" >> pomiar_zad_2.txt
	time -p ./zad2 a test2 2>> pomiar_zad_2.txt
	echo "500kB file:" >> pomiar_zad_2.txt
	time -p ./zad2 a test3 2>> pomiar_zad_2.txt
	make sys
	echo "##### SYS #####" >> pomiar_zad_2.txt
	echo "10kB file:" >> pomiar_zad_2.txt
	time -p ./zad2 a test 2>> pomiar_zad_2.txt
	echo "100kB file:" >> pomiar_zad_2.txt
	time -p ./zad2 a test2 2>> pomiar_zad_2.txt
	echo "500kB file:" >> pomiar_zad_2.txt
	time -p ./zad2 a test3 2>> pomiar_zad_2.txt

clean:
	rm -f pomiar_zad_2.txt zad2