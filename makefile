CFLAGS= -g `pkg-config gtk+-3.0 --cflags`

all: libwave.a build/libwave_test

libwave.a: build/wavelib.o
	ar cr libwave.a build/wavelib.o

build/wavelib.o: build_dir src/wavelib.c src/wavelib.h
	gcc $(CFLAGS) -c src/wavelib.c -o build/wavelib.o

build/libwave_test: build/wavelib_test.o
	gcc build/wavelib_test.o -l wave -L . `pkg-config gtk+-3.0 --libs` -o build/libwave_test

build/wavelib_test.o: src/wavelib_test.c src/wavelib.h
	gcc $(CFLAGS) -c src/wavelib_test.c -o build/wavelib_test.o 

build_dir:
	mkdir -p build
	
clean:
	rm build/*