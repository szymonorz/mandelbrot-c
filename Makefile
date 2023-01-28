LDFLAGS += -lSDL2 -lm -lpthread
all: build

build: mandelbrot.o main.o
	${CC} ${LDFLAGS} -o main main.o mandelbrot.o
main.o: main.c mandelbrot.h
	${CC} -c main.c
mandelbrot.o: mandelbrot.c mandelbrot.h
	${CC} -c mandelbrot.c
clean:
	rm *.o main
run:
	./main
