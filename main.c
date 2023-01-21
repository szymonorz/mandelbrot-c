#include <SDL2/SDL.h>
#include <stdlib.h>
#include <error.h>
#include <pthread.h>
#include "mandelbrot.h"
#define WIDTH 1024
#define HEIGHT 1024
#define ITERATIONS 500

double SCALE =  0.0075;
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Color fg = {128, 128, 128, SDL_ALPHA_OPAQUE};
SDL_Color bg = {0, 0, 0, SDL_ALPHA_OPAQUE};

double pixel_map[HEIGHT * WIDTH];

typedef struct __range
{
    int start;
    int end;
} range;


typedef struct __vec{
    double min;
    double max;
} vec;

// typedef struct __thread_args{
//     vec Re;
//     vec Im;
//     range r;
// }

double
lerp (vec v, double t)
{
    return (1.0 - t) * v.min + t * v.max;
}

vec Re = {-2.5 , 1};
vec Im = {-1 , 1};

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

    double newReMin = newX - (Re.max - Re.min) / 2 / zoom;
    Re.max = newX + ( Re.max - Re.min ) / 2 / zoom;
    Re.min = newReMin;

    double newImMin = newY - (Im.max - Im.min) / 2 / zoom;
    Im.max = newY + ( Im.max - Im.min ) / 2 / zoom;
    Im.min = newImMin;
}

int number_of_threads = 12;

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
            // newX = Re.min + ((Re.max - Re.min) * x/WIDTH);
            newX = lerp(Re, tX);
            // newY = Im.min + ((Im.max - Im.min) * y/HEIGHT);
            newY = lerp(Im, tY);
            double complex c = CMPLX(newX, newY);
            pixel_map[x + y * WIDTH] = escape_count(c, ITERATIONS)/(double)ITERATIONS;
        }
    }
}


void
compute_parallel(pthread_t* threads, range* ranges)
{
    int tid=0;
    for(tid = 0; tid < number_of_threads; tid++)
    {
        ranges[tid].start = (HEIGHT/number_of_threads)*tid;
        ranges[tid].end = (HEIGHT/number_of_threads)*(tid+1);
        pthread_create(&threads[tid], NULL, &mandelbrot_thread, &ranges[tid]);
    }

    for(tid = 0; tid < number_of_threads; tid++)
    {
        pthread_join(threads[tid], 0);
    }
}

int
main(int argc, char* argv[])
{
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
            fprintf(stderr, "Failed to init SDL2 %s", SDL_GetError());
            return EXIT_FAILURE;
    }

    window = SDL_CreateWindow("Window",
                                     SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED,
                                     WIDTH,
                                     HEIGHT,
                                     SDL_WINDOW_OPENGL
                                     );

    if(window == NULL)
    {
        fprintf(stderr, "Unable to create window: %s", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

    //Draw loop
    SDL_Event event;

    int running = 1;
    int x, y;

    pthread_t threads[number_of_threads];
    range ranges[number_of_threads];
    compute_parallel(threads, ranges);
    while(running)
    {
        SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
        SDL_RenderClear(renderer);


        for(y=0; y<HEIGHT; y++)
        {
            for(x=0; x<WIDTH; x++)
            {
                // if(!pixel_map[x + y*WIDTH] > 0)
                // {
                    double color =  (1.0 - pixel_map[x + y*HEIGHT]) * 255.0;

                    SDL_SetRenderDrawColor(renderer, color, color, color, fg.a);
                    SDL_RenderDrawPoint(renderer, x, y);
                    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
                // }
            }
        }
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                running = 0;
                break;
            }
            else if(event.type == SDL_MOUSEWHEEL)
            {
                if(event.wheel.y > 0) //Zoom out
                {
                    zoom(5.0);
                }
                else if(event.wheel.y < 0) //Zoom in
                {
                    zoom(0.2);
                }

                compute_parallel(threads, ranges);
            }

        }

        SDL_RenderPresent(renderer);
    }

    //Closing
    if(window != NULL)
    {
        SDL_DestroyWindow(window);
        window = NULL;
    }

    if(renderer != NULL)
    {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }

    SDL_Quit();
    return EXIT_SUCCESS;

}
