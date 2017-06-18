/* fd-write.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main( int argc, char * argv[] )
{
  int fd = open( argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0660 );
                                                     /* mode */

  /* for the octal number 0660 above, the binary form is 110 110 000 */
  /*                                                     rwx rwx rwx */
  /*                                                   user group other */

  /* fd should be 3 here, because the fd table already has entries
      for 0 (stdin), 1 (stdout), 2 (stderr) */

  if ( fd == -1 )
  {
    perror( "open() failed" );   /* prints to stderr */
    return EXIT_FAILURE;
  }

  printf( "fd for file %s is %d\n", argv[1], fd );

  int rc = write( fd, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26 );
  printf( "write() wrote %d bytes to fd %d\n", rc, fd );


  close( fd );  /* remove entry 3 from the fd table */

  return EXIT_SUCCESS;
}

