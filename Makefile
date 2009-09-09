CC=gcc
CFLAGS=-g -O2

all: dcookie fd_bmsocket fd_udpsocket fd_fusemount

dcookie: dcookie.c
	gcc dcookie.c -o exec/dcookie

fdpass.o: fdpass.c
	gcc -c fdpass.c -o fdpass.o

fd_bmsocket: fd_bmsocket.c fdpass.o
	gcc fd_bmsocket.c fdpass.o -o exec/fd_bmsocket

fd_udpsocket: fd_udpsocket.c fdpass.o
	gcc fd_udpsocket.c fdpass.o -o exec/fd_udpsocket

fd_fusemount: fd_fusemount.c fdpass.o
	gcc fd_fusemount.c fdpass.o -o exec/fd_fusemount

clean: 
	rm -f exec/*
