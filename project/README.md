# Project

## Summary

This directory contains the files for the overall project.

## Development

### Dependencies

This project uses Bison and Flex (latest versions of each). It has not been
designed with yacc or lex, so I can't guarantee whether they will work.
Doxygen is an optional dependency, but is not required if you don't want to
read the documentation, which is generated from the function comments.

You can install flex and bison using homebrew or your desired package
manager.

### Build instructions

This project is set up with `CMake`. To build this project, do an
out-of-source build. I usually use a directory called `build`.

Example:
```sh
mkdir -p build;
cd build;
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```
You can also run `build.sh`, which will perform the same steps.
```sh
./build.sh
```
This will create a binary in the build folder called `minic`.

You can invoke it with `./build/minic`. It will take input from
`stdin`.

### Documentation

Developer documentation is generated through `Doxygen`.

To generate the docs:
```sh
doxygen Doxyfile
```
It will provide warnings for elements that are not documented.

If you want to use the modern theme:
```sh
dox2html5 Doxyfile-mcss
```
The CMake file also contains information so Doxygen documentation can be
generated through the Cmake build system. It will create a target called
`docs` that will invoke doxygen and output documentation in the build
folder.

If you're in the build folder, you can run
```sh
make docs
```
(or whichever build system you're using), and it will generate the HTML output
in `docs/html`. The entry point is `docs/html/index.html`.
