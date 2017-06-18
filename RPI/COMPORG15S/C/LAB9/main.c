#include <stdio.h>
#include "pi.h"
#include "trig.h"
int main()
{
	double ang = 90.0 * pi/180.00; /* 90 Degrees in Radians */
	printf("cotan of 90 degrees: %0.4f\n", ctn(ang) );
	printf("cotan of 45 degrees: %0.4f\n", ctn(ang/2) );
	return 0;
}