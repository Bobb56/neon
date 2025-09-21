cd ../../..
make -f Makefile.linux_amd64
cp bin/neon_amd64 build/linux/snap/neon
cd build/linux/snap
snapcraft pack
snapcraft upload neon-interpreter_4.0_amd64.snap
