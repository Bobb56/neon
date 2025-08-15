#!/bin/sh
gcc -O3 -fno-fast-math -ftrapping-math -fsigned-zeros -frounding-math -m64 \
    ../../src/*.c ../../src/extern/*.c ../../src/*.S \
    -lm \
    -o Neon/usr/local/bin/neon \
&& \
dpkg-deb --build Neon
