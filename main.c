#include "mandelbrot.h"
#include <unistd.h>

SDL_Window* window;
SDL_Renderer* renderer;

void
usage(char* name)
{
    fprintf(stderr, "Usage: %s [-w WIDTH] [-h HEIGHT] [-i ITERATIONS] [-t NUMBER OF THREADS] [-d]\n", name);
    exit(EXIT_FAILURE);
}

int
main(int argc, char* argv[])
{
    int opt;
    int thread_num = 4;
    iterations = 128; WIDTH = 512; HEIGHT = 512; DEBUG = 0;
    while((opt = getopt(argc, argv, "h:w:i:t:d")) != -1)
    {
        switch(opt)
        {
            case 'h': HEIGHT = atoi(optarg); break;
            case 'w': WIDTH = atoi(optarg); break;
            case 'i': iterations = atoi(optarg); break;
            case 't': thread_num = atoi(optarg); break;
            case 'd': DEBUG = 1; break;
            default: usage(argv[0]); break;
        }
    }

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
            fprintf(stderr, "Failed to init SDL2 %s", SDL_GetError());
            return EXIT_FAILURE;
    }

    window = SDL_CreateWindow("Mandelbrot set SDL2",
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
     if(renderer == NULL)
    {
        fprintf(stderr, "Unable to create renderer: %s", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }
    if(DEBUG)
    {
        fprintf(stderr,
                "Created window of size: %dx%d.\n"
                "Using %d threads\n"
                "Iterations per pixel: %d\n",
                WIDTH, HEIGHT,
                thread_num,
                iterations);
    }

    pixel_map = malloc(sizeof(SDL_Color) * HEIGHT * WIDTH);

    //Draw loop
    SDL_Event event;

    int running = 1;
    int x, y;

    compute_parallel(thread_num);
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
                if(event.wheel.y > 0) zoom(2.0); //Zoom out
                else if(event.wheel.y < 0) zoom(0.5); //Zoom in

                compute_parallel(thread_num);
            }
            else if(event.type == SDL_KEYDOWN)
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_q: {
                        // Clamping
                        iterations = iterations != 1 ? iterations/2: 1;
                    } break;
                    case SDLK_e: iterations*=2;break;
                }
                compute_parallel(thread_num);
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
    if(DEBUG) fprintf(stderr, "Quiting...\n");

    free(pixel_map);
    SDL_Quit();
    return EXIT_SUCCESS;
}
