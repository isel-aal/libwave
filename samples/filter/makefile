all: wave_filter

CFLAGS = `pkg-config libwave --cflags` -g -Wall -Werror -pedantic

LIBS = `pkg-config libwave --libs`

wave_filter: wave_filter.o
	gcc wave_filter.o $(LIBS) -o wave_filter

wave_filter.o: wave_filter.c
	gcc $(CFLAGS) -c wave_filter.c

clean:
	rm -rf *.o *.i wave_filter
