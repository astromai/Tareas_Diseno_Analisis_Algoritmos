CC = gcc
CFLAGS = -O2 -Wall

all: main

main: main.c mergesort.c quicksort.c io_utils.c
	$(CC) $(CFLAGS) -o main main.c mergesort.c quicksort.c io_utils.c

run: main
	./main

clean:
	rm -f *.o main *.bin
