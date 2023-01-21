#include "mandelbrot.h"

int
in_mandelbrot(double complex c, int num_of_iterations)
{
    return stability(c, num_of_iterations);
}

float
stability(double complex c, int num_of_iterations)
{
    return (float)escape_count(c, num_of_iterations)/(float)num_of_iterations;
}

int
escape_count(double complex c, int num_of_iterations)
{
    int i;
    double complex z = CMPLX(0,0);
    for(i=0; i<num_of_iterations; i++)
    {
            if(cabsf(z) > 2)
            {
                return i;
            }

            z = z*z + c;
    }
    return num_of_iterations;
}
