CC=gcc
CFLAGS=-Wall -g # -std=c99
LDFLAGS=
LDLIBS=

all: phreak1 phreak2 phreak3
phreak1: main1.o phreak.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)
phreak2: main2.o phreak.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)
phreak3: main3.o phreak.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

main1.o: main.c phreak.h
	$(CC) -DPART=1 $(CFLAGS) -c -o $@ $<
main2.o: main.c phreak.h
	$(CC) -DPART=2 $(CFLAGS) -c -o $@ $<
main3.o: main.c phreak.h
	$(CC) -DPART=3 $(CFLAGS) -c -o $@ $<
phreak.o: phreak.c phreak.h

clean:
	-rm -rf phreak.o main1.o main2.o main3.o phreak1 phreak2 phreak3

view:
	vim phreak.c phreak.h -o
view_all:
	vim phreak.c phreak.h phreak_limits.h -o
handin:
	echo "" > output.txt && handin -o cs317 a1 > handin.txt
test:
	cat test/part1-sample.input | ./phreak1

test_all:
	cat test/part1-sample.input | ./phreak1 > test1.txt && \
	diff test1.txt test/part1-sample.output >> test1.txt && \
	cat test/part2-sample.input | ./phreak2 > test2.txt && \
	diff test2.txt test/part2-sample.output >> test2.txt && \
	cat test/part1-sample.input | ./phreak3 > test3.txt && \
	diff test3.txt test/part3-sample.output >> test3.txt
	
