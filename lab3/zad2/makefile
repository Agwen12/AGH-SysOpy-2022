
comp:
	gcc -Wall main.c -o zad2

clean:
	rm -f zad2 w* report_2.txt

del_w:
	rm -f w*.txt

show:
	grep "" w*

report:
	make clean
	make comp
	./zad2 0.1 1 >> report_2.txt
	make del_w
	./zad2 0.01 10 >> report_2.txt
	make del_w
	./zad2 0.001 100 >> report_2.txt
	make del_w
	./zad2 0.0001 1000 >> report_2.txt
	make del_w
	./zad2 0.00001 10000 >> report_2.txt
	make del_w
	./zad2 0.0000001 10000 >> report_2.txt
	make del_w

