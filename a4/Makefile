CC=gcc
#CFLAGS=-Wall -g -Wextra -Wno-unused-parameter
#CFLAGS=-Wall -Werror -g -Wextra -Wno-unused-parameter
CFLAGS=-Wall -g -Wextra 
LDFLAGS=

all: cshttp
cshttp: cshttp.o service.o util.o
test_util: test_util.o util.o

cshttp.o: cshttp.c service.h
service.o: service.c service.h util.h
util.o: util.c util.h
test_util.o: test_util.c util.h

clean:
	-rm -rf cshttp.o service.o util.o cshttp
