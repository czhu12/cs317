CC=gcc
CFLAGS=-Wall -g # -Wextra -Wno-unused-parameter
LDFLAGS=
LDLIBS=

all: eth_switch1 eth_switch2
eth_switch1: eth_switch1.o main1.o
eth_switch2: eth_switch2.o main2.o

main1.o: main1.c eth_switch1.h eth_switch_limits.h
main2.o: main2.c eth_switch2.h eth_switch_limits.h
eth_switch1.o: eth_switch1.c eth_switch1.h eth_switch_limits.h
eth_switch2.o: eth_switch2.c eth_switch2.h eth_switch_limits.h

clean:
	-rm -rf main1.o main2.o eth_switch1.o eth_switch2.o eth_switch1 eth_switch2