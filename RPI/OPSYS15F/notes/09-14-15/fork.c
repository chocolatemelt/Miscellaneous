/* fork.c */

/* create a child process */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main()
{
  int x = 5;
  pid_t pid;     /* process id (pid)  unsigned int/short */

  pid = fork();  /* create a child process */

  /* this fork() system call, if successful, will
      essentially "return twice" -- once in the parent process,
       and once in the child process */

  /* the entire parent memory space is copied and is used as
      the child process's memory space

      things that get duplicated/copied in the child process:
      -- program counter (PC)
      -- variables and their values (i.e., anything in memory)
      -- pointers and what they point to
      -- file descriptors (fd table)
      -- environment variables
      -- process priority
      -- controlling terminal
      -- current working directory
      -- signal handlers.......
      -- etc.

      things that do NOT get duplicated:
      -- process id (pid)
      -- parent process id (ppid)      bash$ ps -f      ps -ef
      -- list of child processes (NULL)
      -- data on resource allocation (CPU usage)
      -- physical memory location(s)
      -- pending signals........
  */

  if ( pid == -1 )
  {
    perror( "fork() failed" );
    return EXIT_FAILURE;
  }

  if ( pid == 0 )  /* child process fork() returns 0 */
  {
    printf( "CHILD: happy birthday! my pid is %d\n", getpid() );
    x += 10;
    printf( "CHILD: x is %d\n", x );

sleep( 10 );
    return 12;
  }
  else /* pid > 0 */  /* parent process fork() returns the child's pid */
  {
/* sleep( 1 ); */
    printf( "PARENT: my child's pid is %d\n", pid );
    x -= 5;
    printf( "PARENT: x is %d\n", x );

    /* wait for the child process to terminate */
    int status;
    pid_t child_pid = wait( &status );
                   /* wait() blocks indefinitely */

    printf( "PARENT: child %d terminated...", (int)child_pid );

    if ( WIFSIGNALED( status ) )   /* core dump or kill or kill -9 */
    {
      printf( "abnormally\n" );
    }
    else if ( WIFEXITED( status ) ) /* child called return or exit() */
    {
      int rc = WEXITSTATUS( status );
      printf( "successfully with exit status %d\n", rc );
    }
  }

  return EXIT_FAILURE;
}
