/* signal-input.c */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void signal_handler( int n )  /* n is the signal rcvd (e.g., SIGINT) */
{
  printf( "Rcvd signal %d\n", n );

  if ( n == SIGUSR1 )
  {
    printf( "Reloading our config file......jk\n" );
  }

  return;
}

int main()
{
  /* change the behavior/disposition for SIGINT */
  signal( SIGINT, SIG_IGN );

  signal( SIGINT, signal_handler ); 
  signal( SIGUSR1, signal_handler );

  char name[100];
  printf( "enter your name ==> " );
  scanf( "%s", name );

  return EXIT_SUCCESS;
}
