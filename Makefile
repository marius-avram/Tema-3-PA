all: build

build: Graph.o Main.o
	g++ -O3 -lm Main.o Graph.o -o tema3

.cpp.o:
	g++ -O3 -funroll-loops -c -Wall -c $? 

clean: 
	-rm -f *.o tema3
run:
	./tema3
	
.PHONY: all clean

