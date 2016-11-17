# Makefile - Cache Simulator (ORGB-2016/1)
# Levindo Neto

all: 243685.o

243685.o: cache_simulator.c
	gcc -Wall -g -o 243685 cache_simulator.c

clean:
	rm -rf *.o *~ *.out 243685 # Remove the .o and the executable
