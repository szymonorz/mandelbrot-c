#ifndef HH_MANDELBROT
#define HH_MANDELBROT

#include <complex.h>

float stability(double complex c, int num_of_iterations);

int escape_count(double complex c, int num_of_iterations);

#endif
