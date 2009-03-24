#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "fdpass.h"

unsigned int rcvbuf = 16*1024*1024;
unsigned int arg_length = 128;

void receive_argument(int control_channel_fd, char *source) {
    int received;
    received=recv(control_channel_fd, source, arg_length);
    if (received<arg_length) {
        printf("Error receiving arguments over the control buffer\n");
        exit(1);
    }
}

int get_magic_fd (char *data) {
    char *ptr;
    int fd;

    data[arg_length-1]='\0';
    ptr = strstr(data,"fd=");
    if (!ptr)
        return -1;

    // Found two fd= expressions
    if (strstr(ptr+3,"fd="))
        return -1;

    if (*(ptr+3)!='\0') {
        sscanf(ptr+3,"%d",&fd);
        return fd;
    }
    else
        return -1;
}

int main(int argc, char *argv[]) {
    int control_channel_fd, magic_fd;
    char source[128],target[128],filesystemtype[128],data[129];

    int received;

    if (argc < 3) {
        printf("This script is called by vsys.\n");
        exit(1);
    }

    sscanf(argv[2],"%d", &control_channel_fd);

    receive_argument(control_channel_fd, source);
    receive_argument(control_channel_fd, target);
    receive_argument(control_channel_fd, filesystemtype);
    receive_argument(control_channel_fd, data);
    
    magic_fd = get_magic_fd (data);

    if (magic_fd < 3) {
        printf("Got fd %d in fusemount\n",magic_fd);
        exit(1);
    }



}
