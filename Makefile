CC=g++
CFLAGS=-O3
LDFLAGS=-lm -lncurses -s

main: main.cc floor.o static_floor.o dynamic_floor.o automaton.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o evacuation main.cc floor.o static_floor.o dynamic_floor.o automaton.o
floor.o: floor.cc floor.h constants.h
	$(CC) $(CFLAGS) -c floor.cc
static_floor.o: static_floor.cc static_floor.h floor.o
	$(CC) $(CFLAGS) -c static_floor.cc
dynamic_floor.o: dynamic_floor.cc dynamic_floor.h floor.o
	$(CC) $(CFLAGS) -c dynamic_floor.cc
automaton.o: automaton.cc automaton.h dynamic_floor.o static_floor.o
	$(CC) $(CFLAGS) -c automaton.cc

clean:
	rm -f *.o evacuation

