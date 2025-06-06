#!/bin/sh

mkdir -p build
CFLAGS="-g3 -I../src/third_party/glfw/include -I../src/third_party/glad/include -I../src/third_party -L../src/third_party/glfw/lib"
LDLIBS="-lglfw3 -lGL -lm"
cd build >/dev/null
cc $CFLAGS $(realpath ../src/cg/main.c) $LDLIBS
cd ..

