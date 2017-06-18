/* fd.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  char buffer[80];
  sprintf( buffer, "ABCDEFGHIJKLMNOPQRSTUVWXYZ" );

  /* write to fd 1 exactly 12 bytes from buffer */
  int rc = write( 1, buffer, 12 );

  if ( rc == 12 )
  {
    printf( "\nThe write() system call worked\n" );
  }
  else if ( rc == -1 )
  {
    perror( "write() failed" );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

