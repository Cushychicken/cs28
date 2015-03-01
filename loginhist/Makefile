#
# makefile for lecture 2
#
CC=gcc
CFLAGS=-Wall

TARGET=whodate

all:
	make clean
	make utmplib.o
	make whodate
	make lastlogin


lastlogin: lastlogin.c utmplib.o
	$(CC) $(CFLAGS) -o lastlogin lastlogin.c utmplib.o

whodate: whodate.c utmplib.o
	$(CC) $(CFLAGS) -o whodate whodate.c utmplib.o

utmplib.o: utmplib.c
	$(CC) $(CFLAGS) -c utmplib.c

clean:
	rm -f *.o whodate lastlogin 
