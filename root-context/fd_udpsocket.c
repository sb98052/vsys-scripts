/* Copy of fd_bmsocket.c that creates UDP sockets instead of TCP sockets. */

#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
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

    /* receive paramaters: rcvbufsize and sndbufsize */
    receive_argument(control_channel_fd, &rcvbufsize, &sndbufsize);
    if (rcvbufsize > MAX_BUFSIZE)
      rcvbufsize = MAX_BUFSIZE;
    if (sndbufsize > MAX_BUFSIZE)
      sndbufsize = MAX_BUFSIZE;

    magic_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (magic_socket == -1) {
      fprintf(stderr, "Error creating socket: %d\n", errno);
      exit(1);
    }

    /* buffer size <= 0 means we should ignore the parameter */
    if (rcvbufsize > 0) {
      if (setsockopt(magic_socket, 
		     SOL_SOCKET, 
		     SO_RCVBUFFORCE, 
		     &rcvbufsize, sizeof(unsigned int))) {
        fprintf(stderr, "Error calling setsockopt for RCVBUFFORCE: %d\n", 
		errno);
        exit(1);
      }
    }
    if (sndbufsize > 0) {
      if (setsockopt(magic_socket, 
		     SOL_SOCKET, 
		     SO_SNDBUFFORCE, 
		     &sndbufsize, sizeof(unsigned int))) {
        fprintf(stderr, "Error calling setsockopt for SNDBUFFORCE: %d\n", 
		errno);
        exit(1);
      }
    }

    send_fd(control_channel_fd, magic_socket);
}
