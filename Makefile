CC=gcc
CFLAGS=-g -O2

all: dcookie fd_bmsocket fd_udpsocket fd_fusemount fd_tuntap fd_tos fd_packetseer fd_netlink fd_bind53

fd_tuntap: fd_tuntap.c
	gcc fd_tuntap.c -o exec/fd_tuntap

fd_netlink: fd_netlink.c fdpass.o
	gcc fd_netlink.c fdpass.o -o exec/fd_netlink

dcookie: dcookie.c
	gcc dcookie.c -o exec/dcookie

fdpass.o: fdpass.c
	gcc -c fdpass.c -o fdpass.o

fd_bind53: fd_bind53.c fdpass.o
	gcc fd_bind53.c fdpass.o -o exec/fd_bind53

fd_bmsocket: fd_bmsocket.c fdpass.o
	gcc fd_bmsocket.c fdpass.o -o exec/fd_bmsocket

fd_udpsocket: fd_udpsocket.c fdpass.o
	gcc fd_udpsocket.c fdpass.o -o exec/fd_udpsocket

fd_fusemount: fd_fusemount.c fdpass.o
	gcc fd_fusemount.c fdpass.o -o exec/fd_fusemount

fd_tos: fd_tos.c fdpass.o
	gcc fd_tos.c fdpass.o -o exec/fd_tos

fd_packetseer: fd_packetseer.c fdpass.o
	gcc fd_packetseer.c fdpass.o -o exec/fd_packetseer
