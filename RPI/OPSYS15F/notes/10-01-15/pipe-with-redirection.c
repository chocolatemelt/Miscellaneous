/* pipe-with-redirection.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  int p[2];  /* array to hold the two pipe file descriptors
                (p[0] is the read end; p[1] is the write end) */

  int rc = pipe( p );

  if ( rc == -1 )
  {
    perror( "pipe() failed" );
    return EXIT_FAILURE;
  }

  /* fd table:

        0: stdin
        1: stdout
        2: stderr
        3: p[0] <=======READ========== (buffer)
        4: p[1] ========WRITE========> (buffer)
   */

  int pid = fork();  /* this will copy the fd table to the child process */

  if ( pid == -1 )
  {
    perror( "fork() failed" );
    return EXIT_FAILURE;
  }

  /* fd table:

        PARENT                                          CHILD
       ------------                                    -------------
        0: stdin                                        0: stdin
        1: stdout                                       1: stdout
        2: stderr                                       2: stderr
        3: p[0] <=======READ========== (buffer) ======> 3: p[0]
        4: p[1] ========WRITE========> (buffer) <====== 4: p[1]
   */

  if ( pid == 0 )
  {
    close( p[0] );   /* close the read end of the pipe */
    p[0] = -1;

    /* write some data to the pipe */
    int bytes_written = write( p[1], "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26 );
    printf( "CHILD: Wrote %d bytes\n", bytes_written );

    int rc = dup2( p[1], 1 );  /* on success, this will return 1
                                     (the second argument)  */
    if ( rc == -1 )
    {
      perror( "dup2() failed" );
      return EXIT_FAILURE;
    }

      /* close fd 1, then copies p[1] to fd 1 in the fd table */
      /* fd table:

        PARENT                                          CHILD
       ------------                                    -------------
        0: stdin                                        0: stdin
        1: stdout                           =========== 1: p[1]
        2: stderr                           v           2: stderr
        3: p[0] <=======READ========== (buffer) ======> 3: p[0]
        4: p[1] ========WRITE========> (buffer) <====== 4: p[1]
       */
    printf( "this stdout should go to the pipe....\n" );
  }
  else /* pid > 0 */
  {
    close( p[1] );  /* close the write end of the pipe */
    p[1] = -1;

/* TO DO: call wait() here..... */

    /* read data from the pipe */
    char buffer[100];
    int bytes_read = read( p[0], buffer, 10 );   /* BLOCKING */
    buffer[bytes_read] = '\0';
    printf( "PARENT: Read %d bytes: %s\n", bytes_read, buffer );

    bytes_read = read( p[0], buffer, 20 );   /* BLOCKING */
    buffer[bytes_read] = '\0';
    printf( "PARENT: Read %d bytes: %s\n", bytes_read, buffer );
  }

  /* fd table:

        PARENT                                          CHILD
       ------------                                    -------------
        0: stdin                                        0: stdin
        1: stdout                           =========== 1: p[1]
        2: stderr                           v           2: stderr
        3: p[0] <=======READ========== (buffer)         3:
        4: p[1]                        (buffer) <====== 4: p[1]
   */

  return EXIT_SUCCESS;
}
