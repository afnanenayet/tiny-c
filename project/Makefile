# Makefile
# Afnan Enayet 2018
CC=clang
IFLAGS=-I include

# need this to use LLVM's stdlib
LDFLAGS=-L/usr/local/opt/llvm/lib -Wl,-rpath,/usr/local/opt/llvm/lib 
FLEXLD=-L/usr/local/opt/flex/lib

.PHONY: all test

test: test/test_compile.c
	$(CC) $^ $(IFLAGS) $(LDFLAGS)
