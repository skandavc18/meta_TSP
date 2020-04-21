a.exe: implementation.o goa.o 
	cl.exe -o goa_p.exe goa.o implementation.o lib/OpenCL.lib

implementation.o: implementation.c defs.h
	cl.exe -c implementation.c

goa.o: goa.c defs.h
	cl.exe -c goa.c

clean: 
	rm -f *.o *.out