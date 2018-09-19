# Short Assignment 1

## Summary

Given C source code, isolate _only_ the comments and print them out.
There should be no other output.

For example, the following source code:

```c
#include <stdio.h>

// main function
int main() {
    /* print function */
    printf("Hello, world!\n");
    return 0;
}
```

Should produce the following output:

```txt
main function
print function
```

## Usage

There is a Makefile included that can generate the program. It uses
`clang` and `flex`. It assumes you have installed `flex` using Homebrew
on MacOS.

To generate the program:

`make`

To clean up:

`make clean`
