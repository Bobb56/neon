cd ../../..
make -f Makefile.linux_riscv64
cp bin/neon_rv64 build/linux/snap/neon
cd build/linux/snap
snapcraft pack
#snapcraft upload neon-interpreter_4.0_riscv64.snap
