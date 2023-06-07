#!/bin/bash

if [ $# -gt 0 ]; then
	base_dir=$1
else
	base_dir=/usr/local
fi
echo "Installing wavelib in" $base_dir
if [ ! -d $base_dir/include ]; then
	mkdir -p $base_dir/include
fi
if [ ! -d $base_dir/lib/pkgconfig ]; then
	mkdir -p $base_dir/lib/pkgconfig
fi

cp libwave.pc $base_dir/lib/pkgconfig
cp build/libwave.a build/libwave.so $base_dir/lib
cp include/wave.h $base_dir/include
