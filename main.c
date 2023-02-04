#include "mandelbrot.h"

void
usage(char* name)
{
    fprintf(stderr, "Usage: %s [-w WIDTH] [-h HEIGHT] [-i ITERATIONS] [-t NUMBER OF THREADS] [-d]\n", name);
    exit(EXIT_FAILURE);
}

int THREADS;
double cY=0, cX=0;

void
update_window_title(SDL_Window * window)
{
    char* window_title = calloc(256,sizeof(char));
    sprintf(window_title, WINDOW_TITLE_FORMAT, WIDTH, HEIGHT, cX, cY, iterations, THREADS);
    SDL_SetWindowTitle(window, window_title);
}

int
main(int argc, char* argv[])
{
    int opt;
    THREADS = 4;
    iterations = 128; WIDTH = 512; HEIGHT = 512; DEBUG = 0;
    while((opt = getopt(argc, argv, "h:w:i:t:d")) != -1)
    {
        switch(opt)
        {
            case 'h': HEIGHT = atoi(optarg); break;
            case 'w': WIDTH = atoi(optarg); break;
            case 'i': iterations = atoi(optarg); break;
            case 't': THREADS = atoi(optarg); break;
            case 'd': DEBUG = 1; break;
            default: usage(argv[0]); break;
        }
    }
    SDL_Window* window;
    SDL_Renderer* renderer;

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
            fprintf(stderr, "Failed to init SDL2 %s", SDL_GetError());
            return EXIT_FAILURE;
    }
    window = SDL_CreateWindow("Window Title",
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

    update_window_title(window);

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
                "[main]:\n"
                "\tCreated window of size: %dx%d.\n"
                "\tUsing %d threads\n"
                "\tIterations per pixel: %d\n",
                WIDTH, HEIGHT,
                THREADS,
                iterations);
    }

    pixel_map = calloc(HEIGHT * WIDTH, sizeof(SDL_Color));

    SDL_Event event;

    int running = 1;
    int x, y;
    vec * axises = calloc(2, sizeof(vec));
    vec Re = {-2.5 , 1};
    vec Im = {-1 , 1};
    axises[0] = Re;
    axises[1] = Im;
    compute_parallel(THREADS);

    // Pętla rysująca
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
                if(event.wheel.y > 0) zoom(2.0, &axises); //Zoom out
                else if(event.wheel.y < 0) zoom(0.5, &axises); //Zoom in

                Re = axises[0];
                Im = axises[1];
                cX = (Re.max - Re.min)/2;
                cY = (Im.max - Im.min)/2;
                compute_parallel(THREADS);
                update_window_title(window);
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
                compute_parallel(THREADS);
                update_window_title(window);
            }
        }
        SDL_RenderPresent(renderer);
    }

    // Zamykanie
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
