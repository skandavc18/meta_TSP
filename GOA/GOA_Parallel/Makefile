goa: implementation.o goa.o 
	gcc -o goa goa.o implementation.o -lOpenCL

implementation.o: implementation.c defs.h
	gcc -c implementation.c

goa.o: goa.c defs.h
	gcc -c goa.c

clean: 
	rm -f *.o *.out