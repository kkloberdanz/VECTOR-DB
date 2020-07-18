CC=cc
STD=-std=gnu89
OPT=-Os
EXTRA=
CFLAGS=-Wall -Wextra -Wpedantic $(STD) $(OPT) -fPIC

.PHONY: release
release: OPT := -O3 -s
release: all

.PHONY: debug
debug: OPT := -O0 -ggdb3
debug: all

.PHONY: all
all: file

file: file.c
	$(CC) -o file file.c $(CFLAGS) $(EXTRA)

.PHONY: clean
clean:
	rm -f file
	rm -f *.o
	rm -f core
