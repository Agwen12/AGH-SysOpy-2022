
comp:
	gcc -Wall main.c -o main -lpthread


test: comp
	for threads in 1 2 4 8 16 32 64 128; do \
  		for var in variant_1 variant_2 ; do \
  		  for img in casablanca.ascii.pgm totem.ascii.pgm ; do \
			./main $$threads $$var ./images/$$img ./negatives/$$img ; \
			done ; \
		  done ; \
	  	done

clean:
	rm -f main
