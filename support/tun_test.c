#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>

#include "tunalloc.h"

int main(void)
{
    printf("Allocating tap device via VSYS\n");

    char if_name[IFNAMSIZ];

    int tun_fd = tun_alloc(IFF_TAP, if_name);

    printf("Allocated tap device: %s fd=%d\n", if_name, tun_fd);

    printf("Sleeping for 120 secs...\n");

    sleep(120);

    printf("Closing\n");

    return 0;
}
