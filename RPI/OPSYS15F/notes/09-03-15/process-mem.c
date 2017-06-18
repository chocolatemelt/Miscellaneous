/* process-mem.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argc, char * argv[] )   /* <=== command-line arguments */
{
  printf( "argc is %d\n", argc );     /*  argument count */

  printf( "argv[0] is %s\n", argv[0] );
  printf( "argv[1] is %s\n", argv[1] );
  printf( "argv[2] is %s\n", argv[2] );



  int x = 5;

  printf( "sizeof int is %d\n", sizeof( int ) );
  printf( "sizeof x is %d\n", sizeof( x ) );


  int * y = &x;

  /* here, y is a statically allocated variable (on the runtime stack) */

  /*                         y:
         +-----+             +------------+
      x: |   5 |<============| <mem-addr> |
         +-----+             +------------+    */

  printf( "sizeof int* is %d\n", sizeof( int* ) );
  printf( "sizeof y is %d\n", sizeof( y ) );
  printf( "sizeof char* is %d\n", sizeof( char* ) );
  printf( "sizeof double* is %d\n", sizeof( double* ) );

  char name[90];   /* this allocates 90 bytes on the stack */


  /* dynamically allocate memory from the heap: */

  y = (int *)malloc( sizeof( int ) );
/*           ^^^^^^^^^^^^^^^^^^^^^^^
              dynamically allocate sizeof( int ) bytes on the heap    */

  if ( y == NULL )
  {
    fprintf( stderr, "malloc() failed" );
    return EXIT_FAILURE;
  }


  char title[12];  /* static memory allocation */
  char xyz[14];
  sprintf( title, "ABCDEFGHIJKLMNOPQRSTUVWXYZ" ); 
  sprintf( xyz, "----------------------" );
  printf( "title is %s\n", title );
  printf( "xyz is %s\n", xyz );

  /*    title: ABCDEFGHIJKLMNOPQRSTUVWXYZ\0
                                         ^^
                                      one character (EOS/NUL/NULL) */

  strcpy( title, "ABCD" );
  printf( "title is %s\n", title );
  printf( "length is %d\n", strlen( title ) );

  /* dynamically allocate 50 int variables (consecutive) from the heap */
  int * array = (int *)malloc( 50 * sizeof( int ) );
  array[0] = 12;
  array[1] = 42;
  array[20] = 49;

  /* preferred way to dynamically allocate an array */
  array = (int *)calloc( 50, sizeof( int ) );

  /* TO DO: call free() to free up this memory */



  return EXIT_SUCCESS;   /* return 0  after the preprocessor runs */
}












