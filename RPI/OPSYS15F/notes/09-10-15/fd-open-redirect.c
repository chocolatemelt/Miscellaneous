/* fd-open-redirect.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
  char name[80];
  strcpy( name, "testfile.txt" );

   /* name: "testfile.txt\0"                         */
   /*                    ^^                          */
   /*                 the '\0' is a single character */

  close( 1 ); /* close stdout */

  int fd = open( name, O_WRONLY | O_CREAT | O_TRUNC );

  if ( fd == -1 )
  {
    perror( "open() failed" );
    return EXIT_FAILURE;
  }


  /*  fd table (BEFORE open() call):
      0 stdin
      1 
      2 stderr

      fd table (AFTER open() call):
      0 stdin
      1 testfile.txt (O_RDONLY)
      2 stderr
  */

  /* this line of output is buffered to go to fd 1,
     which is testfile.txt */
  printf( "fd for file %s is %d\n", name, fd );

  /* the above line of output will not get to the file
     unless the following fflush() call is made: */
  fflush( NULL );


  char buffer[81];
  sprintf( buffer, "WRITING TO FD %d\n", fd );
  int rc = write( fd, buffer, strlen( buffer ) );
  printf( "write() returned %d\n", rc );

  if ( rc == -1 )
  {
    perror( "write() failed" );
    return EXIT_FAILURE;
  }


  close( fd );  /* remove entry 1 from the fd table */

  return EXIT_SUCCESS;
}

