#sh
cd ../../..
make -f Makefile.linux_amd64 -j8
cd -
lpm build neon.yaml
