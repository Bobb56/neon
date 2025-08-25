del neon.exe
del ressources.res
windres.exe ressources.rc -O coff -o ressources.res
cd ../../
gcc -O3 -fno-fast-math -ftrapping-math -fsigned-zeros -frounding-math -m64 src/*.c src/extern/*.c src/*.S build/windows/ressources.res -lm -o build/windows/neon.exe
cd build/windows

iscc neon.iss