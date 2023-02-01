#ifndef HH_MANDELBROT
#define HH_MANDELBROT

#include <SDL2/SDL.h>
#include <stdlib.h>
#include <error.h>
#include <pthread.h>
#include <complex.h>
#include <time.h>
#define H 16 // number of colors

typedef struct __range
{
    int start;
    int end;
    int iterations;
} range;

typedef struct __vec
{
    double min;
    double max;
} vec;

extern int iterations;
extern int WIDTH, HEIGHT;
extern int DEBUG;
extern SDL_Color * pixel_map;
extern vec Re;
extern vec Im;
extern SDL_Color bg;

int escape_count(double complex c, double complex * z);
SDL_Color escape_color(int escape, double complex z);

void zoom(double zoom);
void * mandelbrot_thread(void *args);
double lerp (vec v, double t);
SDL_Color clerp(SDL_Color color1, SDL_Color color2, double t);
void compute_parallel(int thread_num);
#endif
