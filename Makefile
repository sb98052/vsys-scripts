CC=gcc
CFLAGS=-g -O2

all: dcookie fd_bmsocket

dcookie: dcookie.c
	gcc dcookie.c -o exec/dcookie

fd_bmsocket: fd_bmsocket.c
	gcc fd_bmsocket.c -o exec/fd_bmsocket

clean: 
	rm -f exec/*
