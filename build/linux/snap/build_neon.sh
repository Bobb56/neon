#!/bin/sh
echo Compilation de Neon... && \
cd ../../../ && \
gcc -O3 -fno-fast-math -ftrapping-math -fsigned-zeros -frounding-math -m64 src/*.c src/extern/*.c src/*.S -lm -o build/linux/snap/neon && \
cd - && \
echo Neon compilé!
