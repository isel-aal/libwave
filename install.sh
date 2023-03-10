#!/bin/bash

if [ $# -ne 1 ]; then
	echo "usage: $0 <install directory>"
	exit
fi

base_dir=$1

if [ ! -d $base_dir/lib/pkgconfig ]; then
	mkdir $base_dir/lib/pkgconfig
fi

cp libwave.pc $base_dir/lib/pkgconfig
cp build/libwave.a build/libwave.so $base_dir/lib
cp include/wave.h $base_dir/include
