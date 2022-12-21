CFLAGS = `pkg-config libwave --cflags` -g -Wall -Werror -pedantic

LIBS = `pkg-config libwave --libs`

all: libwave_test

libwave_test: libwave_test.o
	gcc libwave_test.o $(LIBS) -o libwave_test

libwave_test.o: libwave_test.c
	gcc $(CFLAGS) -c libwave_test.c -o libwave_test.o

clean:
	rm *.o libwave_test
