CC=gcc
CFLAGS=-g -O2

all: dcookie fd_bmsocket

dcookie: dcookie.c
	gcc dcookie.c -o exec/dcookie

fdpass.o: fdpass.c
	gcc -c fdpass.c -o fdpass.o

fd_bmsocket: fd_bmsocket.c fdpass.o
	gcc fd_bmsocket.c fdpass.o -o exec/fd_bmsocket

clean: 
	rm -f exec/*
