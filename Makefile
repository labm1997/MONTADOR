montador: main.o
	g++ main.o -o montador
main.o: main.cpp
	g++ -c main.cpp
clean: 
	rm *.o montador
