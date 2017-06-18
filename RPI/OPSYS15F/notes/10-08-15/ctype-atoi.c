/* ctype-atoi.c */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main()
{
  char buffer[100];
  sprintf( buffer, "%d)", 5123 );

  int n = atoi( buffer );

  printf( "Converted \"%s\" to %d\n", buffer, n );


  if ( isdigit( buffer[1] ) )
  {
    printf( "'%c' is a digit\n", buffer[1] );
  }

  if ( isalpha( buffer[1] ) )    /* isalnum(), ispunct(), isspace(), etc. */
  {
    printf( "'%c' is an alpha character\n", buffer[1] );
  }

  return EXIT_SUCCESS;
}
