

run_tests: comp
	./zad1 fork ignore
	./zad1 fork handle
	./zad1 fork mask
	./zad1 fork pending

	./zad1 exec ignore
	./zad1 exec handle
	./zad1 exec mask
	./zad1 exec pending


comp:
	gcc -Wall main.c -o zad1
	gcc -Wall tester.c -o t1

clean:
	rm -f zad1 tester report_1.txt t1