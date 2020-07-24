CC=cc
STD=-std=gnu89
OPT=-Os
EXTRA=
LDFLAGS=-lm
CFLAGS=-Wall -Wextra -Wpedantic $(STD) $(OPT) -fPIC

.PHONY: release
release: OPT := -O3 -s
release: all

.PHONY: debug
debug: OPT := -O0 -ggdb3
debug: all

.PHONY: all
all: a.out

a.out: file ktmath
	$(CC) -o a.out *.o $(CFLAGS) $(EXTRA) $(LDFLAGS)

file: file.c file.h ktmath.h
	$(CC) -o file.o -c file.c -DKT_TEST_MMAP $(CFLAGS) $(EXTRA)

ktmath: ktmath.c ktmath.h
	$(CC) -o ktmath.o -c ktmath.c $(CFLAGS) $(EXTRA)

.PHONY: clean
clean:
	rm -f file
	rm -f *.o
	rm -f core
