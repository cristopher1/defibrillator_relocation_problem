main: main.cpp algoritmo.o
	g++ -Wall -g -std=c++11 -o proyecto main.cpp algoritmo.o

algoritmo.o: algoritmo.cpp algoritmo.h
	g++ -Wall -g -std=c++11 -c -o algoritmo.o algoritmo.cpp
