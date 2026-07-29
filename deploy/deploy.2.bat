cd ..
make -f Makefile.windows_amd64 clean
make -f Makefile.windows_amd64 -j8
copy bin\neon.exe deploy\Neon_%1\neon_%1_amd64.exe

cd build\windows
iscc neon.iss
move neon_%1_windows_amd64_setup.exe ..\..\deploy\Neon_%1\neon_%1_windows_amd64_setup.exe

cd ..\..\deploy