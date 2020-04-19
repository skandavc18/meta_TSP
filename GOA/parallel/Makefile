a.out: implementation.o goa.o 
	gcc implementation.o goa.o -framework OpenCL -o goa_p

implementation.o: implementation.c defs.h
	gcc -c implementation.c

goa.o: goa.c defs.h
	gcc -c goa.c

clean: 
	rm -f *.o *.out