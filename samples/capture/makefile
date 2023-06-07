all: wave_capture

CFLAGS = `pkg-config libwave --cflags` -g -Wall -Werror -pedantic

LIBS = `pkg-config libwave --libs` -lasound -pthread

wave_capture: wave_capture.o
	gcc wave_capture.o $(LIBS) -o wave_capture

wave_capture.o: wave_capture.c
	gcc $(CFLAGS) -c wave_capture.c

clean:
	rm -rf *.o *.i wave_capture
