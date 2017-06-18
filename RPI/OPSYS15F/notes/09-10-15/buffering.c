/* buffering.c */

#include <stdio.h>
#include <stdlib.h>

int main()
{
  int * xyz;

  printf( "HERE" );
  *xyz = 123;   /* segfault occurs here */

  /* to see "HERE" displayed, use "HERE\n" instead
     (the '\n' character flushes the buffered output) */

  return EXIT_SUCCESS;
}
