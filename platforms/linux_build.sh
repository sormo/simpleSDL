#!/bin/sh

mkdir build_linux
cd build_linux
cmake -DCMAKE_BUILD_TYPE:STRING=Debug ../..
make
cd ..
