CC = gcc
CFLAGS = -std=c11 -Wall -O2

SRCS = main.c external_mergesort.c external_quicksort.c io_utils.c
OBJS = $(SRCS:.c=.o)
DEPS = external_mergesort.h external_quicksort.h io_utils.h

EXEC = sorter

.PHONY: all clean run

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(EXEC)

clean:
	rm -f *.o $(EXEC) *.bin merge_* quick_* part_*.bin sorted_*.bin temp_*.bin
