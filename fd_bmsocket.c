#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "fdpass.h"

unsigned int rcvbuf = 16*1024*1024;

int main(int argc, char *argv[]) {
    int control_channel_fd, magic_socket;
    if (argc < 3) {
        printf("This script is called by vsys.\n");
        exit(1);
    }

    sscanf(argv[2],"%d", &control_channel_fd);

    magic_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (magic_socket == -1) {
        printf("Error creating socket: %d\n", errno);
        exit(1);
    }
    if (setsockopt(magic_socket, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(unsigned int))) {
        printf("Error calling setsockopt: %d\n", errno);
        exit(1);
    }
    send_fd(control_channel_fd, magic_socket);
}
