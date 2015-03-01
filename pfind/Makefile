#
# makefile for homework 2
#
CC=gcc
CFLAGS=-Wall

all:
	make pfind

pfind: pfind.c searchdir.o
	$(CC) $(CFLAGS) -o pfind pfind.c searchdir.o

searchdir.o: searchdir.c 
	$(CC) $(CFLAGS) -c searchdir.c -o searchdir.o 

clean:
	rm -f *.o pfind 
