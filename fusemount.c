#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "fuse.h"

void remove_string( char *data, char *string ) {
  char *pos = strstr( data, string );
  if( pos != NULL ) {
    char *comma = strchr( pos, ',' );
    if( comma == NULL ) {
      char *endpos = pos - 1;
      if( endpos < data ) {
	endpos = data;
      }
      endpos[0] = '\0';
    } else {
      char tmp[200];
      strcpy( tmp, comma+1 );
      strcpy( pos, tmp );
    }
  }
}

int do_mount( int control_channel ) {
  char buf[1024];
  int n = read( control_channel, buf, 1024 );
  assert( n != 1024 );

  char source[200];
  char target[200];
  char fstype[200];
  unsigned long mountflags;
  char data[200];
  
  int r = sscanf( buf, "%s\n%s\n%s\n%ld\n%s\n", source, target, fstype, 
		  &mountflags, data );

  assert( r == 5 ); // shouldn't be an assert

  int s = mount( source, target, fstype, mountflags, data );

  // TODO: should probably make sure this is for a fuse filesystem type . . .
  int fd = 0;
  if( s != 0 && errno == EINVAL ) {
    // replace fd with a new one
    remove_string( data, "fd" );

    char *dev;
    fd = open_fuse_device(&dev);
    char *end = data+strlen(data);
    if( strlen(data) != 0 ) {
      sprintf( end, "," );
      end = end + 1;
    }
    sprintf( end, "fd=%d", fd );

    s = mount( source, target, fstype, mountflags, data );

  }

  char outbuf[10];
  if( s < 0 ) {
    sprintf( outbuf, "%08x\n", errno );
  } else {
    sprintf( outbuf, "%08x\n", fd );
  }
  write( control_channel, outbuf, strlen(outbuf) );

  if( fd != 0 ) {

    // now we have to sendmsg it back
    // send the fd
    int r = send_fd(control_channel, fd);

    // wait for the close
    if( r == 0 ) {
      int x = read(control_channel, outbuf, 1 );
      assert( x <= 0 );
    }

    close(fd);

  }

}

int do_umount( int control_channel, int num ) {

  if( num > 30 ) {
    return -1;
  }
    
  char *argbuf = (char *) malloc(sizeof(char)*(1024*num+1));
  const char *args[num+1];
  int args_index = 0;

  // read all of the arguments
  int last_o = 0;
  do {

    int n = read( control_channel, argbuf, 1024*num+1 );
    assert( n < 1024*num+1 ); // no assert here
    int num_left = n;
    while( num_left > 0 ) {
      char arg[1024];
      if( sscanf( argbuf, "%s\n", arg ) != 1 ) {
	break;
      }
      int len = strlen(arg);
      char *a = malloc( sizeof(char)*(strlen(arg)+1) );
      strcpy( a, arg );
      if( strcmp( a, "-o" ) == 0 ) {
	last_o = 1;
      }
      args[args_index] = a;
      num_left -= (len+1);
      argbuf += (len+1);
      args_index++;
    }

  } while( args_index < num );

  args[num] = NULL;

  // The last argument is the mountpoint, that's really all we care about
  printf( "umounting %s\n", args[num-1] );
  int r = fuse_mnt_umount("reroutemount_server", args[num-1], 0 );

  // write the status (probably not necessary)
  char outbuf[10];
  if( r < 0 ) {
    sprintf( outbuf, "%d\n", errno );
  } else {
    sprintf( outbuf, "%d\n", 0 );
  }
  write( control_channel, outbuf, strlen(outbuf) );

}

int main( int argc, char **argv ) {
    int control_channel;
    if (argc < 3) {
        printf("This script is invoked by vsys\n");
        exit(1);
    }

    sscanf(argv[2],"%d",&control_channel);
    
    // read the length
    char buf[1024];
    int n = read( control_channel, buf, 9 );
    buf[n] = '\0';
    if( n != 9 ) {
      close(control_channel);
      continue;
    }

    int num;
    if( sscanf( buf, "%08x\n", &num ) != 1 ) {
      close(control_channel);
      continue;
    }

    if( num == 0 ) {
      do_mount( control_channel );
    } else {
      do_umount( control_channel, num );
    }
    close(control_channel);

  }

  close(fd);
  unlink(socket_name);

}
