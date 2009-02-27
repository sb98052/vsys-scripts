CC=gcc
CFLAGS=-g -O2

all: dcookie

dcookie: dcookie.c
	gcc dcookie.c -o exec/dcookie

clean: 
	rm -f exec/*
