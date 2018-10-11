# Project

## Summary

This directory contains the files for the overall project.

## Development

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

(or whichever build system you're using)

## TODO

- [ ] semantic analysis functions
- [ ] tree walk helper functions
