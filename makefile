CFLAGS= `pkg-config glib-2.0 --cflags` -I include -g -Wall -Werror -pedantic

all: build/libwave.a build/libwave.so

build/libwave.a: build/wave.o
	ar cr build/libwave.a build/wave.o

build/wave.o: build_dir src/wave.c include/wave.h
	gcc $(CFLAGS) -c src/wave.c -o build/wave.o

build/libwave.so: build/wave_pic.o
	gcc -shared build/wave_pic.o -o build/libwave.so

build/wave_pic.o: build_dir src/wave.c include/wave.h
	gcc $(CFLAGS) -fPIC -c src/wave.c -o build/wave_pic.o

build_dir:
	mkdir -p build

clean:
	rm build/*
