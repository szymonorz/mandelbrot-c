LDFLAGS += -lSDL2 -lm -lpthread
CFLAGS += -Wall -Werror -pedantic
all: build

build: mandelbrot.o main.o
	${CC} ${LDFLAGS} ${CFLAGS} -o main main.o mandelbrot.o
main.o: main.c mandelbrot.h
	${CC} -c main.c
run-benchmark: benchmark
	./benchmark ${EXEC_FLAGS}
benchmark: benchmark.o mandelbrot.o
	${CC} ${LDFLAGS} ${CFLAGS} -o benchmark benchmark.o mandelbrot.o
benchmark.o: benchmark.c mandelbrot.h
	${CC} -c benchmark.c
mandelbrot.o: mandelbrot.c mandelbrot.h
	${CC} -c mandelbrot.c
clean:
	rm *.o main benchmark
run:
	./main

