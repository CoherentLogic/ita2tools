CC=gcc
CFLAGS=-std=c99
OBJS=libita2.o
EXE=libita2.a

all: libita2.a

libita2.a: libita2.o
	ar rcs libita2.a libita2.o

libita2.o: libita2.c
	$(CC) $(CFLAGS) -c libita2.c -o libita2.o

clean:
	rm -f $(OBJS) $(EXE)
