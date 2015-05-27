all:
	g++ -c -std=c++11 *.cpp
	gcc -c -std=gnu11 -g -Wall -O3 *.c
	g++ -o a.out *.o

