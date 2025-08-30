cd ../../..
make -f Makefile.linux
cp bin/neon build/linux/snap/
cd build/linux/snap
snapcraft pack
#snapcraft upload neon-interpreter_4.0_amd64.snap
