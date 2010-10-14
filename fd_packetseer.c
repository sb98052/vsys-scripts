#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <linux/if_ether.h>
#include "fdpass.h"

#define MAX_BUFSIZE (32*1024*1024)

/*------------------------------------------------------------------*/
void
receive_argument(int control_fd, int *rcvbuf, int *sndbuf)
{
  if (recv(control_fd, rcvbuf, sizeof(int), 0) != sizeof(int)) {
    fprintf(stderr, "receiving the first argument failed\n");
    exit(-1);
  }
  if (recv(control_fd, sndbuf, sizeof(int), 0) != sizeof(int)) {
    fprintf(stderr, "receiving the first argument failed\n");

    exit(-1);
  }
}
/*------------------------------------------------------------------*/
int 
main(int argc, char *argv[]) 
{
    int control_channel_fd, magic_socket;
    int rcvbufsize = 0, sndbufsize = 0;
    
    if (argc < 3) {
        printf("This script is called by vsys.\n");
        exit(1);
    }

    sscanf(argv[2],"%d", &control_channel_fd);
    magic_socket = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (magic_socket == -1) {
      fprintf(stderr, "Error creating socket: %d\n", errno);
      exit(1);
    }
    else fprintf(stderr, "Socket: %d", magic_socket);

    send_fd(control_channel_fd, magic_socket);
}
