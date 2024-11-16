#!/bin/bash

##
## PROJECT:     BootSectorInstaller
## LICENSE:     MIT (https://spdx.org/licenses/MIT)
## PURPOSE:     A simple configuration script thats makes building/configuring project more easier
## COPYRIGHT:   Copyright 2024 Daniel Victor <ilauncherdeveloper@gmail.com>
##

RECONFIG="0"
BUILD="0"
BUILD_TYPE="Release"

for arg in "$@"
do
    shift
    if [ "$arg" == "-build" ]; then
        BUILD="1"
    fi
    if [ "$arg" == "-reconfigure" ]; then
        RECONFIG="1"
    fi
    if [ "$arg" == "-build-type" ]; then
        BUILD_TYPE="$1"
    fi
done

if [ "$RECONFIG" == "1" ]; then
    rm -rf "output"
fi

cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -B "output" -S .

if [ "$BUILD" == "1" ]; then
    cmake --build "output"
fi
