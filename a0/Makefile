CC=gcc
CFLAGS=-Wall -g
LDFLAGS=

all: map
map: main.o map.o

main.o: main.c map.h
map.o: map.c map.h

clean:
	-rm -rf map.o main.o map