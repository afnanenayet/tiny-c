# Project 2

## Summary

This is the subdirectory for project #2 for CS 57. It is a program that takes
LLVM IR and performs the following optimizations:

* constant propagation
* constant folding

## Building

This project was built using `CMake` and should be generally compatible with
any system that has the LLVM libraries installed.

To build the project:

```sh
mkdir build
cd build
cmake ..
make
```

There is also a convenience script that will perform the same actions for you:

```sh
./build.sh
```

These will produce a binary called `optimizer` in the `build` directory.

If you want to build the documentation, you will need to install Doxygen, and
in `CMakeLists.txt`, find the this line:

```cmake
option(enable_docs "Generate documentation with Doxygen" OFF)
```

And change it to:

```cmake
option(enable_docs "Generate documentation with Doxygen" ON)
```

There is also a debug flag which will enable debugging statements that get
printed to `stdout`. You can enable these by building in debug mode. It will
also enable standard debug flags and export debugging information so that you
can easily analyze the binary using `gdb` or `lldb`.

```sh
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make
```
