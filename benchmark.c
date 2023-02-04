#include "mandelbrot.h"
#include <unistd.h>

void
usage(char* name)
{
    fprintf(stderr, "Usage: %s [-w WIDTH] [-h HEIGHT] [-i ITERATIONS] [-t MAX_THREADS] [-d]\n", name);
    exit(EXIT_FAILURE);
}

void
generate_filename(char* filename)
{
    time_t t = time(NULL);
    struct tm *  tt = localtime(&t);
    if(tt == NULL)
    {
        fprintf(stderr, "Error calling localtime\n");
        exit(EXIT_FAILURE);
    }
    strftime(filename, 256, "result_%Y%m%d%H%M%S", tt);
}

int
main(int argc, char* argv[])
{
    iterations = 1024; WIDTH = 1024; HEIGHT = 1024;
    int MAX_THREADS=4;
    int opt;
    char* table_header = "\"threads\" \"execution_time\"\n";
    char* table_row = malloc(256 * sizeof(char));
    char* filename = malloc(256  * sizeof(char));
    generate_filename(filename);
    FILE* result_file;
    while((opt = getopt(argc, argv, "h:w:i:t:d:f:")) != -1)
    {
        switch(opt)
        {
            case 'h': HEIGHT = atoi(optarg); break;
            case 'w': WIDTH = atoi(optarg); break;
            case 'i': iterations = atoi(optarg); break;
            case 't': MAX_THREADS = atoi(optarg); break;
            case 'd': DEBUG = 1; break;
            case 'f': {
                if(strlen(optarg) > strlen(filename))
                   filename = realloc(filename, strlen(optarg) * sizeof(char));
                strcpy(filename, optarg);
            }break;
            default: usage(argv[0]); break;
        }
    }
    printf("Performing a benchmark on compute_parallel function....\n");
    printf("Benchmark results will be written to %s\n", filename);
    result_file = fopen(filename, "w");
    fwrite(table_header, sizeof(char), strlen(table_header)*sizeof(char), result_file);
    pixel_map = malloc(sizeof(SDL_Color) * HEIGHT * WIDTH);
    int thread_num = 1;
    int i;
    clock_t start_time, finish_time;
    double execution_time = 0;
    int bytes;
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
            execution_time = ((double)(finish_time - start_time)/CLOCKS_PER_SEC)/thread_num;
            bytes = sprintf(table_row, "\"%d\" \"%f\"\n", thread_num, execution_time);
            fwrite(table_row, sizeof(char), bytes*sizeof(char), result_file);
        }
    }
    fclose(result_file);
    free(filename);

    return EXIT_SUCCESS;
}
