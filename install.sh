#!/bin/bash

base_dir=/usr/local

if [ ! -d $base_dir/lib/pkgconfig ]; then
	mkdir $base_dir/lib/pkgconfig
fi

cp libwave.pc $base_dir/lib/pkgconfig
cp build/libwave.a build/libwave.so $base_dir/lib
cp include/wave.h $base_dir/include
