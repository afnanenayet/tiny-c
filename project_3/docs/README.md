# Part 3 (Register Allocation)

## Summary

This program takes in LLVM IR, which is parsed using the LLVM API,
performs linear scan register allocation, and emits GCC assembly.
It works on a subset of allowed input, and implements the following
instructions:

- `jmp` and variants (`jne`, `jg`, etc)
- `movl`
- `addl`
- `subl`
- `mull`

and likely more that I can't remember (the arithmetic, `mov`, and comparison
instructions are the major parts of the program).

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
