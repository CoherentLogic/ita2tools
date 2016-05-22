CC=gcc
CFLAGS=-std=c99
OBJS=ita2.o
EXE=ita2

all: ita2

ita2: ita2.o
	$(CC) $(CFLAGS) -o $(EXE) ita2.o

ita2.o: ita2.c
	$(CC) $(CFLAGS) -c ita2.c

clean:
	rm -f $(OBJS) $(EXE)
