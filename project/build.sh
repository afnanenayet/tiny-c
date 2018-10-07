# build.sh
# Afnan Enayet 2018
#
# Creates a build directory and runs CMake

echo "Creating build directory...";
mkdir -p build;
cd build;

echo "Generating Makefiles using CMake...";
cmake .. -DCMAKE_BUILD_TYPE=Release;

echo "Building project...";
make;
