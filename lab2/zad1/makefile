

lib:
	gcc -Wall main.c -o zad1 -D LIB


sys:
	gcc -Wall main.c -o zad1 -D SYS

tests:
	make lib
	echo "##### LIB #####" >> pomiar_zad_1.txt
	echo "10kB file:" >> pomiar_zad_1.txt
	time -p ./zad1 test out1 2>> pomiar_zad_1.txt
	echo "100kB file:" >> pomiar_zad_1.txt
	time -p ./zad1 test2 out2 2>> pomiar_zad_1.txt
	echo "500kB file:" >> pomiar_zad_1.txt
	time -p ./zad1 test3 out3 2>> pomiar_zad_1.txt
	make sys
	echo "##### SYS #####" >> pomiar_zad_1.txt
	echo "10kB file: \t" >> pomiar_zad_1.txt
	time -p ./zad1 test out4 2>> pomiar_zad_1.txt
	echo "100kB file: \t" >> pomiar_zad_1.txt
	time -p ./zad1 test2 out5 2>> pomiar_zad_1.txt
	echo "500kB file:" >> pomiar_zad_1.txt
	time -p ./zad1 test3 out6 2>> pomiar_zad_1.txt


clean:
	rm -f zad1 out* pomiar_zad_1.txt
