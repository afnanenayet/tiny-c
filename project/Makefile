# Makefile
# Afnan Enayet 2018
CC=clang
IFLAGS=-I include
WFLAGS=-Wpedantic -Wall -std=c11

# need this to use LLVM's stdlib
LDFLAGS=-L/usr/local/opt/llvm/lib -Wl,-rpath,/usr/local/opt/llvm/lib

# link the flex libs
FLEXLD=-L/usr/local/opt/flex/lib

.PHONY: all test

test: test/test_compile.o
	$(CC) $^ $(IFLAGS) $(LDFLAGS) $(WFLAGS)

test/test_compile.o: test/test_compile.c
	$(CC) $(IFLAGS) $^ -c -o $@

clean:
	rm -f test/*.o test/a.out  # test files
