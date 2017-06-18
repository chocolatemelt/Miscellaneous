#include <stdio.h>
#include <math.h>
#include <stdlib.h>

/* Introduction to OpenMP: Computes trapezoidal */
/* approximation to an integral */

const double pi = 3.141592653589793238462643383079;
double f(double); /* function definition */

int main(int argc, char** argv)
{
	double a = 0.0, b = pi; /* limits of integration */;
	int n = 4194304; /* number of subdivisions = 2^22 */
	double h = (b - a) / n; /* width of subdivision */
	double integral; /* accumulates answer */
	long threadct = 1; /* number of threads to use */
	int i; /* loop variable */

	/* parse command-line arg for number of threads */
	if (argc > 1)
		threadct = strtol(argv[1],NULL,10);

#ifdef _OPENMP
	printf("OMP defined, threadct = %lu\n", threadct);
#else
	printf("OMP not defined\n");
#endif

	integral = (f(a) + f(b))/2.0; /* initialize variable integral */
#pragma omp parallel for num_threads(threadct) \
    shared (a, n, h) private(i) reduction(+: integral)
    for(i = 1; i < n; i++)
	{
		integral += f(a+i*h);
	}
	integral = integral * h;
	printf("With n = %d trapezoids, our estimate ", n);
	printf("of the integral from %g to %g is %g\n", a, b, integral);
}

double f(double x)
{
	return sin(x);
}
