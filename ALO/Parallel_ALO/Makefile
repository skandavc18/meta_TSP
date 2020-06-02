a.out: implementation.o alo.o 
	gcc -o alo alo.o implementation.o -lOpenCL

implementation.o: implementation.c defs.h
	gcc -c implementation.c

alo.o: alo.c defs.h
	gcc -c alo.c

clean: 
	rm -f *.o *.out