/* 
 * vsys.c: CPython functions to wrap PlanetLab vsys API'
 *   Alina Quereilhac - 02/09/2012
 *
 * Copyright (c) 2012 INRIA
 *
 */

#include <Python.h>
#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif

#include <errno.h>
#include <fcntl.h>
#include <linux/if_tun.h>
#include <linux/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define VSYS_TUNTAP "/vsys/fd_tuntap.control"
#define VSYS_VIFUP_IN "/vsys/vif_up.in"
#define VSYS_VIFUP_OUT "/vsys/vif_up.out"
#define VSYS_VIFDOWN_IN "/vsys/vif_down.in"
#define VSYS_VIFDOWN_OUT "/vsys/vif_down.out"
#define VSYS_VROUTE_IN "/vsys/vroute.in"
#define VSYS_VROUTE_OUT "/vsys/vroute.out"

const size_t SIZE = 4096; 

int _fd_tuntap(int if_type, int no_pi, char *if_name);
int _vif_up(const char *if_name, const char *ip, const char *prefix, int snat,
        char *msg);
int _vif_down(const char *if_name, char *msg);
int _vroute (const char *action, const char *network, const char *prefix, 
        const char *host, const char *device, char *msg);

/* Python wrapper for _fd_tuntap */
static PyObject *
fd_tuntap(PyObject *self, PyObject *args) {
    int if_type, fd, no_pi = 0;
    char *if_name;
    PyObject *retval;

    if(!PyArg_ParseTuple(args, "i|i", &if_type, &no_pi))
        return NULL;

    if((if_name = malloc(SIZE)) == NULL)
        return PyErr_SetFromErrno(PyExc_OSError);

    // just in case, initialize the buffer to 0
    memset(if_name, 0, SIZE);

    Py_BEGIN_ALLOW_THREADS;
    fd = _fd_tuntap(if_type, no_pi, if_name);
    Py_END_ALLOW_THREADS;

    retval = Py_BuildValue("is", fd, if_name);
    free(if_name);
    return retval;
}

/* Python wrapper for _vif_up */
static PyObject *
vif_up(PyObject *self, PyObject *args) {
    int ret, snat = 0; //false
    char *if_name, *ip, *prefix, *msg;
    PyObject *retval;

    if(!PyArg_ParseTuple(args, "sss|i", &if_name, &ip, &prefix, &snat))
        return NULL;

    if((msg = malloc(SIZE)) == NULL)
        return PyErr_SetFromErrno(PyExc_OSError);

    // just in case, initialize the buffer to 0
    memset(msg, 0, SIZE);

    Py_BEGIN_ALLOW_THREADS;
    ret = _vif_up(if_name, ip, prefix, snat, msg);
    Py_END_ALLOW_THREADS;

    retval = Py_BuildValue("is", ret, msg);
    free(msg);
    return retval;
}

/* Python wrapper for _vif_down */
static PyObject *
vif_down(PyObject *self, PyObject *args) {
    int ret;
    char *if_name, *msg;
    PyObject *retval;

    if(!PyArg_ParseTuple(args, "s", &if_name))
        return NULL;

    if((msg = malloc(SIZE)) == NULL)
        return PyErr_SetFromErrno(PyExc_OSError);

    // just in case, initialize the buffer to 0
    memset(msg, 0, SIZE);

    Py_BEGIN_ALLOW_THREADS;
    ret = _vif_down(if_name, msg);
    Py_END_ALLOW_THREADS;

    retval = Py_BuildValue("is", ret, msg);
    free(msg);
    return retval;
}

/* Python wrapper for _vroute */
static PyObject *
vroute(PyObject *self, PyObject *args) {
    int ret;
    char *action, *network, *prefix, *host, *device, *msg;
    PyObject *retval;

    if(!PyArg_ParseTuple(args, "sssss", &action, &network, &prefix, &host, &device))
        return NULL;

    if((msg = malloc(SIZE)) == NULL)
        return PyErr_SetFromErrno(PyExc_OSError);

    // just in case, initialize the buffer to 0
    memset(msg, 0, SIZE);

    Py_BEGIN_ALLOW_THREADS;
    ret = _vroute(action, network, prefix, host, device, msg);
    Py_END_ALLOW_THREADS;

    retval = Py_BuildValue("is", ret, msg);
    free(msg);
    return retval;
}

static PyMethodDef methods[] = {
    {"fd_tuntap", fd_tuntap, METH_VARARGS, "(fd, if_name) = fd_tuntap(if_type, no_pi = False)"},
    {"vif_up", vif_up, METH_VARARGS, "(code, msg) = vif_up(if_name, ip, prefix, snat = False)"},
    {"vif_down", vif_down, METH_VARARGS, " (code, msg) = vif_up(if_name)"},
    {"vroute", vroute, METH_VARARGS, "(code, msg) = vroute(action, network, prefix, host, device)"},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC init_vsys(void) {
    PyObject *m;
    m = Py_InitModule("_vsys", methods);
    if (m == NULL)
        return;
}

/*
 * _fifo_push(): Pushes parameters into a fifo.
 *   
 *   Parameters:
 *     fifo_in:    the name of the fifo to write parameters to
 *     fifo_out:   the name of the fifo to read error from 
 *     input:      parameters formated as a string to push into the input fifo
 *     msg:        buffer to return error to user
 *
 *  Return value:
 *    On success, _fifo_push returns 0.
 *    On error, a negative integer is returned.
 *
 */
static int _fifo_push (const char *fifo_in, const char *fifo_out, 
        const char *input, char *msg) {
  FILE *in;
  FILE *out;
  int nbytes;

  in = fopen (fifo_in, "a");

  if (in == NULL){
    snprintf (msg, SIZE, "Failed to open FIFO %s", fifo_in);
    return  -1;
  }

  out = fopen (fifo_out, "r");

  if (out == NULL){
    snprintf (msg, SIZE, "Failed to open FIFO %s", fifo_out);
    return  -2;
  }

  // send inputs to the fifo_in
  fprintf (in, "%s", input);

  // force flush and close the fifo_in so the program on the other side
  // can process input.
  fclose (in);

  nbytes = fread(msg, SIZE, 1, out);
 
  // the error buffer will not be empty if we read an error
  if ((nbytes > 0) && (strcmp(msg, "") != 0)) {
      // an errror was read from the fifo_out ...
      return -3;
  }

  fclose (out);

  return 0;
}

/*
 * _receive_fd(): Receives the file descriptor associated to the virtual
 *  device
 *   
 *  Parameters:
 *    vsys_sock:    the vsys control socket which send the fd
 *    if_name:      the buffer where the device name assigned by PlanetLab 
 *                      will be stored after creating the virtual interface 
 *
 *  Return value:
 *    On success, _receive_fd returns the file descriptor associated to the device.
 *    On error, a negative integer is returned.
 *
 */
static int _receive_fd(int vsys_sock, char *if_name) {
  struct msghdr msg;
  struct iovec iov;
  size_t ccmsg[CMSG_SPACE (sizeof(int)) / sizeof(size_t)];
  struct cmsghdr *cmsg;
  int rv;

  // Use IOV to read interface name
  iov.iov_base = if_name;
  iov.iov_len = IFNAMSIZ;

  msg.msg_name = 0;
  msg.msg_namelen = 0;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

 // old BSD implementations should use msg_accrights instead of msg_control.
 // the interface is different.
  msg.msg_control = ccmsg;
  msg.msg_controllen = sizeof(ccmsg);

  while (((rv = recvmsg (vsys_sock, &msg, 0)) == -1) && errno == EINTR)
    {}

  cmsg = CMSG_FIRSTHDR (&msg);
  if (cmsg->cmsg_type != SCM_RIGHTS){
    snprintf (if_name, SIZE, "Got control message of unknown type");
    return -1;
  }

  int* retfd  = (int*)CMSG_DATA (cmsg);
  return *retfd;
}

/*
 * _fd_tuntap(): Creates a TAP or TUN device in PlanetLab, and returns the
 * device name and the associated file descriptor.
 *   
 * Parameters:
 *   if_type:     the type of virtual device. Either IFF_TAP (0x0001) or
 *                      IFF_TUN (0x0002)
 *   no_pi:	  set flag IFF_NO_PI
 *   if_name:     the buffer where the device name assigned by PlanetLab 
 *                      will be stored after creating the virtual interface.
 *                      When an error ocurrs, if_name is used as message buffer
 *
 * Return value:
 *   On success, _fd_tuntap returns the file descriptor associated to the device.
 *   On error, a negative integer is returned.
 *
 */
int _fd_tuntap(int if_type, int no_pi, char *if_name) {
  int vsys_sock, fd;
  struct sockaddr_un addr;

  vsys_sock = socket (AF_UNIX, SOCK_STREAM, 0);

  if (vsys_sock == -1){
    snprintf (if_name, SIZE, "%s", strerror(errno));
    return -2;
  }

  memset (&addr, 0, sizeof(struct sockaddr_un));

  addr.sun_family = AF_UNIX;
  strncpy (addr.sun_path, VSYS_TUNTAP, sizeof(addr.sun_path) - 1);

  int ret = connect (vsys_sock, (struct sockaddr *) &addr, 
    sizeof(struct sockaddr_un));

  if (ret == -1){
    snprintf (if_name, SIZE, "%s", strerror(errno));
    return -3;
  }
 
  // send vif type (either TAP or TUN)
  ret = send (vsys_sock, &if_type, sizeof(if_type), 0);

  if (ret != sizeof(if_type)){
    snprintf (if_name, SIZE, "Could not send parameter to vsys control socket");
    return -4; 
  }

  // receive the file descriptor associated to the virtual interface
  fd = _receive_fd (vsys_sock, if_name);

  if (fd < 0){
    snprintf (if_name, SIZE, "Invalid file descriptor");
    return -5;
  }
 
/**
*  XXX: For now unsupported functionality
*
  struct ifreq ifr;
  ret = ioctl (fd, TUNGETIFF, (void *)&ifr);
  if (ret == -1){
    close (fd);
    snprintf (if_name, SIZE, "%s", strerror(errno));
    return -6;
  }
  
  // flag IFF_NO_PI: don't add the extra PI header
  if (no_pi){
    ifr.ifr_flags |= IFF_NO_PI;
  }

  // flag IFF_ONE_QUEUE: disable normal network device queue and use only the
  // tun/tap driver internal queue. QoS tools will not be
  // able to control queueing for the device in this case.
  ifr.ifr_flags |= IFF_ONE_QUEUE;

  ret = ioctl (fd, TUNSETIFF, (void *)&ifr);
  if (ret == -1){
    close (fd);
    snprintf (if_name, SIZE, "%s", strerror(errno));
    return -7;
  }
*/

  return fd;

}

/*
 * _vif_up(): Configures a virtual interface with the values given by the user
 * and sets its state UP 
 *
 * Parameters:
 *   if_name:   the name of the virtual interface 
 *   ip:        the IP address to be assigned to the interface 
 *   prefix:    the network prefix associated to the IP address
 *   snat:      whether to enable SNAT on the virtual interface.
 *   msg:       buffer to return error to user
 *
 * Return value:
 *   On success, return 0. 
 *   On error, a negative integer is returned.
 *
 */
int _vif_up (const char *if_name, const char *ip, const char *prefix, 
        int snat, char *msg) {

  char input[SIZE]; // big buffer

  snprintf (input, SIZE, "%s\n%s\n%s\nsnat=%d\n", if_name, ip, prefix, snat);

  return _fifo_push (VSYS_VIFUP_IN, VSYS_VIFUP_OUT, input, msg);

}

/*
 * _vif_down(): Sets the state of a virtual interface DOWN 
 *
 * Parameters:
 *   if_name:   the name of the virtual interface 
 *   msg:       buffer to return error to user
 *
 * Return value:
 *   On success, return 0. 
 *   On error, a negative integer is returned.
 *
 */
int _vif_down (const char *if_name, char *msg) {

  char input[SIZE];

  snprintf(input, SIZE, "%s\n", if_name);

  return _fifo_push (VSYS_VIFDOWN_IN, VSYS_VIFDOWN_OUT, input, msg);

}

/*
 * vroute() : Adds or removes routes on PlanetLab virtual interfaces (TAP/TUN).
 *
 *   Note that all networks and gateways must belong to the virtual 
 *   network segment associated to the vsys_vnet tag for the slice.
 *
 *  Parameters:
 *    action:     either 'add' or 'del'
 *    network:    destintation network
 *    prefix:     destination network prefix
 *    host:       IP of gateway virtual interface
 *    device:     name of the gateway virtual interface
 *    msg:        buffer to return error to user
 *
 *  Return value:
 *    On success, vroute returns 0.
 *    On error, a negative integer is returned.
 *
 */
int _vroute (const char *action, const char *network, const char *prefix, 
    const char *host, const char *device, char *msg) {

  char input[SIZE];

  snprintf(input, SIZE, "%s %s/%s gw %s %s\n", action, network, prefix, host, device);

  return _fifo_push (VSYS_VROUTE_IN, VSYS_VROUTE_OUT, input, msg);

}

