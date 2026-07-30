#!/bin/bash

platform=$1
if [[ $1 = "-" ]]; then
    platform="linux_amd64"
elif [[ $1 = "" ]]; then
    echo "$0 <platform> [clean | debug | tests | run]"
    echo "Platforms are supposed to be linux_amd64, windows_amd64, linux_riscv64 or ez80"
    echo "You can use - instead of linux_amd64 for the platform"
    exit
fi


if [[ $2 = "clean" ]]; then
    make -f Makefile.$platform clean
    mkdir obj obj/extern obj/assets 2> /dev/null
elif [[ $2 = "debug" ]]; then
    make -f Makefile.$platform -j 8 debug
elif [[ $2 = "tests" ]]; then
    if [[ $platform = "linux_amd64" ]]; then
        cd tests/bugs
        bash tests.sh log $1
        cd -
    fi
elif [[ $2 = "run" ]]; then
    if [[ $platform = "linux_amd64" ]]; then
        make -f Makefile.$platform -j 8 && \
        valgrind --leak-check=full --track-origins=yes bin/neon
    elif [[ $platform = "windows_amd64" ]]; then
        make -f Makefile.$platform -j 8 && \
        bin\neon
    elif [[ $platform = "ez80" ]]; then
        make -f Makefile.$platform -j 8 && \
        echo "To run Neon on ez80, open CEmu and install the app"
    else
        make -f Makefile.$platform -j 8 && \
        bin/neon
    fi
else
    make -f Makefile.$platform -j 8
    if [[ $platform = "ez80" ]]; then
        convbin --iformat 8ek --input bin/Neon.8ek --oformat 8xv-split --maxvarsize 65200 --output bin/Neon.8xv --name Neon
    fi
fi
