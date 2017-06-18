all: clcg4.h clcg4.c assignment3.c
	gcc -g -I. -Wall -O3 -c clcg4.c -o clcg4.o
	mpicc -g -I. -Wall -O3 assignment3.c clcg4.o -o assignment3
