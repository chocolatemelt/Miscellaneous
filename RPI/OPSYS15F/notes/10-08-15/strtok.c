/* strtok.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
  char * word;
  char * unused;
  char * expr = "(* 5 (+ 15 12) 18)";

  char temp[1024];
  strcpy( temp, expr );   /* we'll make a copy, because strtok()
                              changes the input */

     /* before: "(* 5 (+ 15 12) 18)" */
  word = strtok_r( temp, " \t", &unused );
     /* after: "(*\05 (+ 15 12) 18)" */

  while ( word != NULL )
  {
    printf( "[%s] %s\n", temp, word );
    word = strtok_r( NULL, " \t", &unused );
     /* after: "(*\05\0(+ 15 12) 18)" */
     /*             ^                 */
  }

  printf( "[%s]\n", temp );

  return EXIT_SUCCESS;
}
