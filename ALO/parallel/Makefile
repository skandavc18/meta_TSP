a.out: implementation.o alo.o 
	gcc implementation.o alo.o -framework OpenCL -o alo_p

implementation.o: implementation.c defs.h
	gcc -c implementation.c

alo.o: alo.c defs.h
	gcc -c alo.c

clean: 
	rm -f *.o *.out