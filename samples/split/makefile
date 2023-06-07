all: wave_split

CFLAGS = `pkg-config libwave --cflags` -I ../wavelib/include -g -Wall -Werror -pedantic

LIBS = `pkg-config libwave --libs`

wave_split: wave_split.o
	gcc wave_split.o $(LIBS) -o wave_split

wave_split.o: wave_split.c
	gcc $(CFLAGS) -c wave_split.c

wave.o: ../wavelib/src/wave.c
	gcc $(CFLAGS) -c ../wavelib/src/wave.c

clean:
	rm -rf *.o *.i wave_split
