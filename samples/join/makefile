all: wave_join

CFLAGS = `pkg-config libwave --cflags` -I ../wavelib/include -g -Wall -Werror -pedantic

LIBS = `pkg-config libwave --libs`

wave_join: wave_join.o
	gcc wave_join.o $(LIBS) -o wave_join

wave_join.o: wave_join.c
	gcc $(CFLAGS) -c wave_join.c

wave.o: ../wavelib/src/wave.c
	gcc $(CFLAGS) -c ../wavelib/src/wave.c

clean:
	rm -rf *.o *.i wave_join
