/* fork-with-signal.c */

/* create a child process */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void kill_zombies( int n )
{
  printf( "Rcvd signal %d\n", n );

  int status;
  int child_pid = wait( &status );
    /* this blocking call to wait() returns immediately */

  printf( "Child pid %d just terminated...\n", child_pid );
}

int main()
{
  signal( SIGCHLD, kill_zombies );

  pid_t pid;     /* process id (pid)  unsigned int/short */

  pid = fork();  /* create a child process */

  if ( pid == -1 )
  {
    perror( "fork() failed" );
    return EXIT_FAILURE;
  }

  if ( pid == 0 )  /* child process fork() returns 0 */
  {
    printf( "CHILD: happy birthday! my pid is %d\n", getpid() );
    return 12;
  }
  else /* pid > 0 */  /* parent process fork() returns the child's pid */
  {
    printf( "PARENT: my child's pid is %d\n", pid );

    sleep( 10 );
    sleep( 10 );
  }

  return EXIT_SUCCESS;
}
