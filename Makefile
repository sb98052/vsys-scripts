CC=gcc
CFLAGS=-g -O2

all: dcookie fd_bmsocket

dcookie: dcookie.c
	gcc dcookie.c -o exec/dcookie

fd_pass.o: fd_pass.c
	gcc -c fd_pass.c -o fd_pass.o

fd_bmsocket: fd_bmsocket.c fdpass.o
	gcc fd_bmsocket.c fd_pass.o -o exec/fd_bmsocket

clean: 
	rm -f exec/*
