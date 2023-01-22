#include "mandelbrot.h"

SDL_Window* window;
SDL_Renderer* renderer;

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

        for(y=1; y<HEIGHT; y++)
        {
            for(x=1; x<WIDTH; x++)
            {
                SDL_Color color = pixel_map[x + y * HEIGHT];
                SDL_SetRenderDrawColor(renderer,
                                       color.r,
                                       color.g,
                                       color.b,
                                       SDL_ALPHA_OPAQUE);
                SDL_RenderDrawPoint(renderer, x, y);
                SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
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
