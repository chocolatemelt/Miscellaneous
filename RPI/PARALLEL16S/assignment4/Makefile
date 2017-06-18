all: clcg4.h clcg4.c assignment4.c
	gcc -g -I. -Wall -O3 -c clcg4.c -o clcg4.o
	mpicc -g -I. -pthread -Wall -O3 assignment4.c clcg4.o -o assignment4
