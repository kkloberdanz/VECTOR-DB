CC=cc
STD=-std=gnu89
OPT=
EXTRA=
LDFLAGS=-lm
CFLAGS=-Wall -Wextra -Wpedantic $(STD) $(OPT) -fPIC

.PHONY: release
release: OPT := -Ofast -march=native -mtune=native -s -flto
release: libvectordb-storage.so libvectordb-storage.a

.PHONY: test
test: OPT := -Ofast -march=native -mtune=native -s -flto
test: a.out

.PHONY: debug
debug: OPT := -O0 -ggdb3
debug: a.out

libvectordb-storage.a: file.o ktmath.o
	ar rcs libvectordb-storage.a file.o ktmath.o

libvectordb-storage.so: file.o ktmath.o
	$(CC) -shared -o libvectordb-storage.so file.o ktmath.o $(CFLAGS) $(EXTRA) $(LDFLAGS)

a.out: main.o libvectordb-storage.a
	$(CC) -o a.out main.o libvectordb-storage.a $(CFLAGS) $(EXTRA) $(LDFLAGS)

main.o: file.h ktmath.h
	$(CC) -o main.o -c main.c $(CFLAGS) $(EXTRA)

file.o: file.c file.h ktmath.h
	$(CC) -o file.o -c file.c $(CFLAGS) $(EXTRA)

ktmath.o: ktmath.c ktmath.h
	$(CC) -o ktmath.o -c ktmath.c $(CFLAGS) $(EXTRA)

.PHONY: clean
clean:
	rm -f file
	rm -f *.o
	rm -f core
	rm -f a.out
	rm -f *.so
	rm -f *.a
