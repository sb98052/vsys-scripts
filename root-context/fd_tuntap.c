/* fd_tuntap.c: VSYS script to allocate slice-local tuntap interfaces.
 *   Thom Haddow - 06/09/09
 *
 * Reads interface type from local control unix socket, replies with fd for new
 * (unconfigured) tuntap interface. VSYS client can get interface name with
 * TUNGETIFF ioctl. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pwd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if.h>
#include <linux/if_tun.h>

int send_vif_fd(int sock_fd, int vif_fd, char *vif_name)
{
	int retval;
	struct msghdr msg;
	struct cmsghdr *p_cmsg;
	struct iovec vec;
	size_t cmsgbuf[CMSG_SPACE(sizeof(vif_fd)) / sizeof(size_t)];
	int *p_fds;


	msg.msg_control = cmsgbuf;
	msg.msg_controllen = sizeof(cmsgbuf);
	p_cmsg = CMSG_FIRSTHDR(&msg);
	p_cmsg->cmsg_level = SOL_SOCKET;
	p_cmsg->cmsg_type = SCM_RIGHTS;
	p_cmsg->cmsg_len = CMSG_LEN(sizeof(vif_fd));
	p_fds = (int *) CMSG_DATA(p_cmsg);
	*p_fds = vif_fd;
	msg.msg_controllen = p_cmsg->cmsg_len;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &vec;
	msg.msg_iovlen = 1;
	msg.msg_flags = 0;

    /* Send the interface name as the iov */
	vec.iov_base = vif_name;
	vec.iov_len = strlen(vif_name)+1;

	while ((retval = sendmsg(sock_fd, &msg, 0)) == -1 && errno == EINTR);
	if (retval != 1) {
		perror("sending file descriptor");
		return -1;
	}
	return 0;
}



int main(int argc, char *argv[])
{
    int control_channel_fd;
    int tap_fd;
    int slice_uid;
    char if_name[IFNAMSIZ];
    int if_type;
    struct ifreq ifr;
    struct passwd *pwd;
   
    if(argc < 3) {
        printf("This script is called by vsys.\n");
        exit(-1);
    }


    /* Get slice UID and control channel fd from VSYS args */ 
    pwd = getpwnam(argv[1]);
    if(pwd==NULL) {
        perror("Failed to lookup UID");
        exit(-1);
    }
    slice_uid = pwd->pw_uid;
    sscanf(argv[2],"%d", &control_channel_fd);

   
     
    /* Get type param from control channel. */
    if(recv(control_channel_fd, &if_type, sizeof(int), 0) != sizeof(int)) {
        perror("fd_tuntap: Failed to read from control channel");
        exit(-1);
    }
    
   
    /* Generate basename for interface */
    if(if_type==IFF_TUN) {
        sprintf(if_name, "tun%d-%%d", slice_uid);
    }
    else if(if_type==IFF_TAP) {
        sprintf(if_name, "tap%d-%%d", slice_uid);
    }
    else { /* TODO: Might also want to allow the other types? */
        fprintf(stderr, "fd_tuntap: %d is not a valid interface type",if_type);
        exit(-1); 
    }

    /* Open tun device */
    if( (tap_fd = open("/dev/net/tun", O_RDWR)) < 0 ) {
        system("modprobe tun");
        system("ln -sf /dev/net/tun /dev/stdtun");
        if( (tap_fd = open("/dev/net/tun", O_RDWR)) < 0 ) {
            perror("ERROR: tun_alloc():open(/dev/net/tun)");
            exit(-1);
        }
    }


    /* Set interface type */
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = if_type; 
    strncpy(ifr.ifr_name, if_name, IFNAMSIZ);
    
    if( ioctl(tap_fd, TUNSETIFF, (void *) &ifr) < 0 ) {
        close(tap_fd);
        perror("fd_tuntap: Failed to set tun type");
    }

    /* Read initialised interface name */
    strcpy(if_name, ifr.ifr_name);


    /* Send tap_fd to slice */
    send_vif_fd(control_channel_fd, tap_fd, if_name);

    return 0;
}
