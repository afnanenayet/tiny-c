#!/usr/bin/env bash
# build.sh
# Afnan Enayet
# Build script to invoke cmake and generate the binary

mkdir -p build
cd build
cmake ..
make
