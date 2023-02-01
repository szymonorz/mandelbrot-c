#include "mandelbrot.h"
#include <unistd.h>

void
usage(char* name)
{
    fprintf(stderr, "Usage: %s [-w WIDTH] [-h HEIGHT] [-i ITERATIONS] [-t MAX_THREADS] [-d]\n", name);
    exit(EXIT_FAILURE);
}

int
main(int argc, char* argv[])
{
    printf("Performing a benchmark on compute_parallel function....\n");
    iterations = 1024; WIDTH = 1024; HEIGHT = 1024;
    int MAX_THREADS=4;
    int opt;
    while((opt = getopt(argc, argv, "h:w:i:t:d")) != -1)
    {
        switch(opt)
        {
            case 'h': HEIGHT = atoi(optarg); break;
            case 'w': WIDTH = atoi(optarg); break;
            case 'i': iterations = atoi(optarg); break;
            case 't': MAX_THREADS = atoi(optarg); break;
            case 'd': DEBUG = 1; break;
            default: usage(argv[0]); break;
        }
    }
    pixel_map = malloc(sizeof(SDL_Color) * HEIGHT * WIDTH);
    int thread_num = 1;
    int i;
    clock_t start_time, finish_time;
    double execution_time, execution_time_avg;
    for(; thread_num <= MAX_THREADS; thread_num++)
    {
        printf("Iterations: %d. Threads: %d\n", iterations, thread_num);
        clock_t start_time = clock();
        execution_time = 0;
        for(i=0; i<10; i++)
        {
            start_time = clock();
            compute_parallel(thread_num);
            finish_time = clock();
            execution_time += ((double)(finish_time - start_time)/CLOCKS_PER_SEC)/thread_num;
        }
        execution_time_avg = execution_time/10;
        fprintf(stderr, "Average time it took to compute the set: %f on %d thread(s)\n", execution_time_avg, thread_num);
    }
}
