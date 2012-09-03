#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <asm/types.h>
#include <linux/netlink.h>

#include "fdpass.h"


static void receive_argument(int control_fd, unsigned int *value)
{
	if (recv(control_fd, value, sizeof(unsigned int), 0) != sizeof(unsigned int)) {
		fprintf(stderr, "receiving argument failed\n");
		exit(-1);
	}
}

int main(int argc, char *argv[]) 
{
    int control_channel_fd, magic_socket;
	struct sockaddr_nl addr;
	unsigned int pid = 0;
	unsigned int gmask = 1;
 
    if (argc < 3) {
        printf("This script is called by vsys.\n");
        exit(1);
    }

    control_channel_fd = atoi(argv[2]);  

	/* receive pid paramater */
	receive_argument(control_channel_fd, &pid);

	/* receive gmask paramater */
	receive_argument(control_channel_fd, &gmask);

    magic_socket =  receive_fd(control_channel_fd);
    if (magic_socket == -1) { 
        fprintf(stderr, "Error creating socket: %d\n", errno);
        exit(1);
    }

	memset(&addr, 0, sizeof(struct sockaddr_nl));
	addr.nl_family = AF_NETLINK;
	addr.nl_pid = pid;
	addr.nl_groups = gmask;
	if (bind(magic_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		fprintf(stderr, "Error calling bind for AF_NETLINK: %d\n", errno);
		exit(1);
	}

    send_fd(control_channel_fd, magic_socket);
}
