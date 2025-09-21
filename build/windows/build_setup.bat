cd ../../
make -f Makefile.windows_amd64
cd build/windows

iscc neon.iss
