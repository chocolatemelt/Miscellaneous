#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

/* function prototypes */
void multMat1( int, float*, float*, float* );
void multMat2( int, float*, float*, float* );
void multMat3( int, float*, float*, float* );
void multMat4( int, float*, float*, float* );
void multMat5( int, float*, float*, float* );
void multMat6( int, float*, float*, float* );

/* Note:  When you call the program without an argument it */
/*  will default to Part A.  The program will run Part B   */
/*  if you pass it any argument */

int main( int argc, char **argv ) {
    int nmax = 1000,i,n;
    
    /* function pointers */
    void (*orderings[])(int,float *,float *,float *) =
       {&multMat1,&multMat2,&multMat3,&multMat4,&multMat5,&multMat6};
    char *names[] = {"ijk","ikj","jik","jki","kij","kji"};

    float *A = (float *)malloc( nmax*nmax * sizeof(float));
    float *B = (float *)malloc( nmax*nmax * sizeof(float));
    float *C = (float *)malloc( nmax*nmax * sizeof(float));

    struct timeval start, end;

    if( argv[1] ) {
        printf("Running Part B...\n\n");

        /* fill matrices with random numbers */
        for( i = 0; i < nmax*nmax; i++ ) A[i] = drand48()*2-1;
        for( i = 0; i < nmax*nmax; i++ ) B[i] = drand48()*2-1;
        for( i = 0; i < nmax*nmax; i++ ) C[i] = drand48()*2-1;

        for( i = 0; i < 6; i++) {
	    /* multiply matrices and measure the time */
	    gettimeofday( &start, NULL );
	    (*orderings[i])( nmax, A, B, C );
	    gettimeofday( &end, NULL );

	    /* convert time to Gflop/s */
	    double seconds = (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
	    double Gflops = 2e-9*nmax*nmax*nmax/seconds;    
	    printf( "%s:\tn = %d, %.3f Gflop/s\n", names[i], nmax, Gflops );
        }
    } else {
        printf("Running Part A...\n\n");

        for( n = 10; n <= nmax; n = n<nmax && n+1+n/3>nmax ? nmax : n+1+n/3 ) {
	    /* fill matrices with random numbers */
	    for( i = 0; i < n*n; i++ ) A[i] = drand48()*2-1;
	    for( i = 0; i < n*n; i++ ) B[i] = drand48()*2-1;
	    for( i = 0; i < n*n; i++ ) C[i] = drand48()*2-1;

	    /* multiply matrices and measure the time */
	    gettimeofday( &start, NULL );
	    multMat1( n, A, B, C );
	    gettimeofday( &end, NULL );

	    /* convert time to Gflop/s */
	    double seconds = (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
	    double Gflops = 2e-9*n*n*n/seconds;    
	    printf( "n = %d, %.3f Gflop/s\n", n, Gflops );
        }
    }

    free( A );
    free( B );
    free( C );

    printf("\n\n");

    return 0;
}

void multMat1( int n, float *A, float *B, float *C ) {
   int i,j,k;
   /* This is ijk loop order. */
   for( i = 0; i < n; i++ )
      for( j = 0; j < n; j++ )
         for( k = 0; k < n; k++ )
            C[i+j*n] += A[i+k*n]*B[k+j*n];
}

void multMat2( int n, float *A, float *B, float *C ) {
   int i,j,k;
   /* This is ikj loop order. */
   for( i = 0; i < n; i++ )
      for( k = 0; k < n; k++ )
         for( j = 0; j < n; j++ )
            C[i+j*n] += A[i+k*n]*B[k+j*n];
}

void multMat3( int n, float *A, float *B, float *C ) {
   int i,j,k;
   /* This is jik loop order. */
   for( j = 0; j < n; j++ )
      for( i = 0; i < n; i++ )
         for( k = 0; k < n; k++ )
            C[i+j*n] += A[i+k*n]*B[k+j*n];
}

void multMat4( int n, float *A, float *B, float *C ) {
   int i,j,k;
   /* This is jki loop order. */
   for( j = 0; j < n; j++ )
      for( k = 0; k < n; k++ )
         for( i = 0; i < n; i++ )
            C[i+j*n] += A[i+k*n]*B[k+j*n];
}

void multMat5( int n, float *A, float *B, float *C ) {
   int i,j,k;
   /* This is kij loop order. */
   for( k = 0; k < n; k++ )
      for( i = 0; i < n; i++ )
         for( j = 0; j < n; j++ )
            C[i+j*n] += A[i+k*n]*B[k+j*n];
}

void multMat6( int n, float *A, float *B, float *C ) {
   int i,j,k;
   /* This is kji loop order. */
   for( k = 0; k < n; k++ )
      for( j = 0; j < n; j++ )
         for( i = 0; i < n; i++ )
            C[i+j*n] += A[i+k*n]*B[k+j*n];
}

