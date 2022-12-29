#include <SDL2/SDL.h>
#include <stdlib.h>
#include <error.h>
#include "mandelbrot.h"
#define WIDTH 1024
#define HEIGHT 1024
#define SCALE 0.0075

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Color fg = {128, 128, 128, SDL_ALPHA_OPAQUE};
SDL_Color bg = {0, 0, 0, SDL_ALPHA_OPAQUE};

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
    while(running)
    {
        SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
        SDL_RenderClear(renderer);
        for(y=0; y<HEIGHT; y++)
        {
            for(x=0; x<WIDTH; x++)
            {
                double complex c = SCALE * CMPLX(x - WIDTH/2.0f, HEIGHT/2.0f - y);
                if(!in_mandelbrot(c, 2000))
                {
                    SDL_SetRenderDrawColor(renderer, fg.r, fg.g, fg.b, fg.a);
                    SDL_RenderDrawPoint(renderer, x, y);
                    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
                }
            }
        }
        while(SDL_PollEvent(&event))
        {
                switch(event.type)
                {
                    case SDL_QUIT:
                    {
                        running = 0;
                        break;
                    }
                    default:
                        break;
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
