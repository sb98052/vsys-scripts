#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "fdpass.h"

int 
main(int argc, char *argv[]) 
{
    int control_channel_fd, magic_socket;
    struct sockaddr_in my_addr;
    
    if (argc < 3) {
        printf("This script is called by vsys.\n");
        exit(1);
    }

    sscanf(argv[2],"%d", &control_channel_fd);

    magic_socket = socket(AF_INET, SOCK_DGRAM, 0);

    if (magic_socket == -1) {
      fprintf(stderr, "Error creating socket: %d\n", errno);
      exit(1);
    }
    
    memset((void *) &my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(53);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(magic_socket, (struct sockaddr *) &my_addr, sizeof(my_addr))==-1) {
        printf("Could not bind to port 53");
        exit(1);
    }

    send_fd(control_channel_fd, magic_socket);
}
