# build.sh
# Afnan Enayet 2018
#
# Creates a build directory and runs CMake

mkdir -p build;
cd build;
cmake ..;
make;
