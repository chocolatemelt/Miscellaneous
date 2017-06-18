/* mountain.c - Generate the memory mountain */
/* Source: Computer Systems by Bryant/OHallaron */
#include <stdlib.h>
#include <stdio.h>
#include "fcyc.h" /* K-best measurement timing routines */
#include "clock.h" /* routines to access the cycle counter */

#define MINBYTES (1 << 10)  /* Working set size ranges from 1 KB */
#define MAXBYTES (1 << 28)  /* ... up to 256 MB */
#define MINSTRIDE 1         /* Strides range from 1 to 24 */
#define MAXSTRIDE 24        
#define MAXELEMS MAXBYTES/sizeof(int)

int data[MAXELEMS];  /* GLOBAL: The array in memory for traversing */

/* function prototypes */
void init_data(int *data, int n);
void test(int elems, int stride);
double run(int size, int stride, double Mhz);

int main()
{
    int size;        /* Working set size (in bytes) */
    int stride;      /* Stride (in array elements) */
    double Mhz;      /* Clock frequency */

    init_data(data, MAXELEMS);  /* Initialize each element in data */
    Mhz = mhz(0, 4);            /* Estimate the CPU clock frequency */
    
    printf("CPU Clock frequency is approx. %.1f MHz\n", Mhz);
    printf("Memory mountain (results below are in MB/sec)\n");

    printf("\n");
   
    /* print header for different strides */
    printf("\t");
    for (stride = 1; stride <= MAXSTRIDE; stride++)
       printf("s%d\t", stride);
   
    printf("\n");
   
    for (size = MAXBYTES; size >= MINBYTES; size >>= 1) {
       /* first column is memory amount */
       if (size > (1 << 20))
          printf("%dm\t", size / (1 << 20));
       else
          printf("%dk\t", size / 1024);
      
       /* run memory access test */
       for (stride = 1; stride <= MAXSTRIDE; stride++) {
          printf("%.1f\t", run(size, stride, Mhz));
       }
       printf("\n");
    }
    exit(0);

}

/* init_data - initializes the array */
void init_data(int *data, int n)
{
    int i;

    for (i = 0; i < n; i++)
	data[i] = n;
}

/* test function - sum the strided elements of the array */
void test(int elems, int stride) /* The test function */
{
    int i, result = 0;
    volatile int sink;

    for (i = 0; i < elems; i += stride)
	result += data[i];
    /* The following line is so the compiler */
    /*   does't optimize the loop away */
    sink = result; /* So compiler doesn't optimize away the loop */
}

/* Run test(elems, stride) and return read throughput (MB/s) */
double run(int size, int stride, double Mhz)
{
    double cycles;
    int elems = size / sizeof(int);
    int test_args[ 2 ];

    test_args[ 0 ] = elems;
    test_args[ 1 ] = stride;

    /* first interation: warm up the cache */
    test(elems, stride);
    /* call test(elems, stide) */                    
    cycles = fcyc2(test, test_args);
    /* convert cycles to MB/s */
    return (size / stride) / (cycles / Mhz); 
}

