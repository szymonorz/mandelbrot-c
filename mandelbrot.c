#include "mandelbrot.h"
int thread_num;
int iterations;
int WIDTH, HEIGHT;
SDL_Color * pixel_map;
vec Re = {-2.5 , 1};
vec Im = {-1 , 1};
SDL_Color bg = {0, 0, 0, SDL_ALPHA_OPAQUE};
SDL_Color colors[H] = {
    {66,   30,   15, SDL_ALPHA_OPAQUE},  // brown 3
    {25,    7,   26, SDL_ALPHA_OPAQUE},  // dark violett
    {9,     1,   47, SDL_ALPHA_OPAQUE}, // darkest blue
    {4,     4,   73, SDL_ALPHA_OPAQUE}, // blue 5
    {0,     7,  100, SDL_ALPHA_OPAQUE},  // blue 4
    {12,   44,  138, SDL_ALPHA_OPAQUE},  // blue 3
    {24,   82,  177, SDL_ALPHA_OPAQUE},  // blue 2
    {57,  125,  209, SDL_ALPHA_OPAQUE},  // blue 1
    {134, 181,  229, SDL_ALPHA_OPAQUE},  // blue 0
    {211, 236,  248, SDL_ALPHA_OPAQUE},  // lightest blue
    {241, 233,  191, SDL_ALPHA_OPAQUE},  // lightest yellow
    {248, 201,   95, SDL_ALPHA_OPAQUE},  // light yellow
    {255, 170,    0, SDL_ALPHA_OPAQUE},  // dirty yellow
    {204, 128,    0, SDL_ALPHA_OPAQUE},  // brown 0
    {153,  87,    0, SDL_ALPHA_OPAQUE},  // brown 1
    {106,  52,    3, SDL_ALPHA_OPAQUE},  //brown 2
};

// interpolacja liniowa
double
lerp (vec v, double t)
{
    return (1.0 - t) * v.min + t * v.max;
}

SDL_Color
clerp(SDL_Color color1, SDL_Color color2, double t)
{
    vec r = {color1.r, color2.r};
    vec g = {color1.g, color2.g};
    vec b = {color1.b, color2.b};
    SDL_Color result = { lerp(r, t), lerp(g,t), lerp(b,t), SDL_ALPHA_OPAQUE };
    return result;
}

int
escape_count(double complex c, double complex * z)
{
    int i;
    for(i=0; i<iterations; i++)
    {
            *z = (*z) * (*z) + c;
            if(cabsf(*z) > 2) break;
    }
    return i;
}

SDL_Color
escape_color(int escape, double complex z)
{
    if(escape < iterations)
    {
        // https://linas.org/art-gallery/escape/smooth.html
        double iter = 1.0 * escape;
        double mu = log(log(cabsf(z)))/log(2);
        iter = iter + 1 - mu;
        int colorId = (int)iter;
        if(iter < 0) iter = 0;
        SDL_Color color1 = colors[colorId % H];
        SDL_Color color2 = colors[colorId % H + 1 < H ? colorId % H + 1 : H];
        return clerp(color1, color2, iter - (int)iter);
    }
    return bg;
}

void
zoom(double zoom)
{
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    double newX, newY;
    double tX = (double)mouseX/WIDTH;
    double tY = (double)mouseY/HEIGHT;

    newX = lerp(Re, tX);
    newY = lerp(Im, tY);

    double newReMin = newX - ((Re.max - Re.min) / 2 / zoom);
    Re.max = newX + (( Re.max - Re.min ) / 2 / zoom);
    Re.min = newReMin;

    double newImMin = newY - ((Im.max - Im.min) / 2 / zoom);
    Im.max = newY + (( Im.max - Im.min ) / 2 / zoom);
    Im.min = newImMin;
}

void *
mandelbrot_thread(void *args)
{
    range r = *((range *) args);
    int y,x;
    double newX, newY;
    for(y=r.start; y<r.end; y++)
    {
        for(x=0; x<WIDTH; x++)
        {
            double tX = (double)x/WIDTH;
            double tY = (double)y/HEIGHT;

            newX = lerp(Re, tX);
            newY = lerp(Im, tY);

            double complex c = CMPLX(newX, newY);
            double complex z = CMPLX(0,0);
            int escape = escape_count(c, &z);
            pixel_map[x + y * WIDTH] = escape_color(escape, z);
        }
    }
}

void
compute_parallel(pthread_t* threads, range* ranges)
{
    int tid=0;
    for(tid = 0; tid < thread_num; tid++)
    {
        ranges[tid].start = (HEIGHT/thread_num)*tid;
        ranges[tid].end = (HEIGHT/thread_num)*(tid+1);
        pthread_create(&threads[tid], NULL, &mandelbrot_thread, &ranges[tid]);
    }

    for(tid = 0; tid < thread_num; tid++)
    {
        pthread_join(threads[tid], 0);
    }
}
