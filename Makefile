CC=gcc
CFLAGS=-Os
DEPS=entropy.h handle_ferr.h
OBJ=entropy.o handle_ferr.o

all: entropy-test entropy-server

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

entropy-test: entropy-test.o entropy.o handle_ferr.o
	$(CC) -o entropy-test entropy-test.o entropy.o handle_ferr.o $(CFLAGS) -lm

entropy-server: entropy-server.o entropy.o
	$(CC) -o entropy-server entropy-server.o entropy.o $(CFLAGS) -lm -levent

clean:
	rm entropy-server entropy-test *.o
