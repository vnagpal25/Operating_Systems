make:
	make compile
	make link

compile: main.cc calculator.cc
	g++ -std=c++17 -Wall -I . -c main.cc calculator.cc

link: main.o calculator.o
	g++ -std=c++17 -Wall -I. main.o calculator.o -o calculate

clean:
	rm *.o
