#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>

#include "fdpass.h"

/*
 * Definitions for IP type of service
 */
#define IPTOS_LOWDELAY    0x10
#define IPTOS_THROUGHPUT  0x08
#define IPTOS_RELIABILITY 0x04
#define IPTOS_MINCOST     0x02
#define IPTOS_NORMALSVC   0x00

static void receive_argument(int control_fd, int *TOS_value)
{
    if (recv(control_fd, TOS_value, sizeof(int), 0) != sizeof(int)) {
        fprintf(stderr, "receiving the IP_TOS argument failed\n");
        exit(-1);
    }
}

int main(int argc, char *argv[]) 
{
    int control_channel_fd, magic_socket;
    int TOS_value = IPTOS_NORMALSVC;
 
    if (argc < 3) {
        printf("This script is called by vsys.\n");
        exit(1);
    }

    control_channel_fd = atoi(argv[2]);  
    
    /* receive IP_TOS paramater */
    receive_argument(control_channel_fd, &TOS_value);

    switch (TOS_value)
    {
        case IPTOS_NORMALSVC:
        case IPTOS_MINCOST:
        case IPTOS_RELIABILITY:
        case IPTOS_THROUGHPUT:
        case IPTOS_LOWDELAY:
	    break;
        default:
            fprintf(stderr, "IP_TOS value not known: %d\n", errno);
            exit(1);
    }

    magic_socket =  receive_fd(control_channel_fd);
    if (magic_socket == -1) { 
        fprintf(stderr, "Error creating socket: %d\n", errno);
        exit(1);
    }

    if (setsockopt(magic_socket, IPPROTO_IP, IP_TOS, &TOS_value, sizeof(TOS_value)) < 0 ) {
        fprintf(stderr, "Error calling setsockopt for IPPROTO_IP: %d\n", errno);
        exit(1);
    }

    send_fd(control_channel_fd, magic_socket);
}
