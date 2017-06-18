/* fork-with-exec.c */

/* create a child process using fork(), then
    execute some command/executable in the child process
     using exec() */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main()
{
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

    /* execute the /bin/ls program in this child process */
    int rc = execlp( "/bin/ls", "ls",    "-a",    NULL );
                   /* path      argv[0], argv[1], etc. */

#if 0
    int rc = execlp( "/bin/lwefoiwefjs", "ls", "-a", NULL );

    int rc = execlp( "/bin/ls", "leoiwjfoeifjs", "-a", NULL );
    
    /* wildcard expansion is up to the shell to do before
        calling exec() */
    int rc = execlp( "/bin/ls", "ls", "-a", "*.c", NULL );

    int rc = execlp( "./a.out", "a.out", NULL );   /* CAREFUL.... */
#endif
    

    printf( "We will (should) never see this output unless exec() failed\n" );
    perror( "execlp() failed" );
    return EXIT_FAILURE;
  }
  else /* pid > 0 */  /* parent process fork() returns the child's pid */
  {
    printf( "PARENT: my child's pid is %d\n", pid );

/*sleep( 10 ); */
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
