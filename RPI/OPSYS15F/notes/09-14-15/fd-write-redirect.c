/* fd-write-redirect.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main( int argc, char * argv[] )
{
  close( 1 ); /* close stdout such that fd table looks like this: 

                          0   stdin
                          1   
                          2   stderr */

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

  /* at this point in the code, the fd table is:

                          0   stdin
                          1   output.txt (O_WRONLY)   argv[1]
                          2   stderr */

  /* this printf() output goes to fd 1... */
  printf( "fd for file %s is %d\n", argv[1], fd );
  /* ...but it is buffered and not in the file yet */
  fflush( NULL );

  int rc = write( fd, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26 );
  printf( "write() wrote %d bytes to fd %d\n", rc, fd );
/*  fflush( NULL ); */


  close( fd );  /* remove entry 3 from the fd table */
   /* note that close() does NOT flush the buffer for you... */

  return EXIT_SUCCESS;
}

