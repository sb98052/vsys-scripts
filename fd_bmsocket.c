#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include "fdpass.h"

#define RCVBUF 16*1024*1024

int main(int argc, char *argv[]) {
    int control_channel_fd, magic_socket;
    if (argc < 3) {
        printf("This script is called by vsys.\n");
        exit(1);
    }

    sscanf(argv[2],"%d", &control_channel_fd);

    magic_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    setsockopt(magic_socket, SOL_SOCKET, SO_RCVBUF, RCVBUF, sizeof(unsigned int));
    send_fd(control_channel, magic_socket);
}
