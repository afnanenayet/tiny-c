# Part 3 (Register Allocation)

## Usage

To use this program, call the program and supply the input file

```sh
./RegAlloc [input file]  # input file is required
```

## Development

To build the program, use the supplied `CMakeLists.txt` file.

```sh
mkdir -p build
cd build
cmake ..
make
```

There is also a convenience shell script that performs the same steps for you,
called `build.sh`

```sh
./build.sh
```

The executable will be generated in the `build` directory (as is standard
convention with CMake).
