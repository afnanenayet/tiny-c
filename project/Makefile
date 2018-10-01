# Makefile
# Afnan Enayet 2018
INCLUDE_DIR=include
TEST_DIR=test

CC=clang
IFLAGS=-I $(INCLUDE_DIR)
WFLAGS=-Wpedantic -Wall -std=c11

# need this to use LLVM's stdlib
LDFLAGS=-L/usr/local/opt/llvm/lib -Wl,-rpath,/usr/local/opt/llvm/lib

# link the flex libs
FLEXLD=-L/usr/local/opt/flex/lib

.PHONY: all test docs clean check

test: $(TEST_DIR)/a.out

$(TEST_DIR)/a.out: $(TEST_DIR)/test_compile.o
	$(CC) $^ $(IFLAGS) $(LDFLAGS) $(WFLAGS) -o $(TEST_DIR)/a.out

$(TEST_DIR)/test_compile.o: $(TEST_DIR)/test_compile.c
	$(CC) $(IFLAGS) $^ -c -o $@

test_clean:
	rm -f $(TEST_DIR)/*.o $(TEST_DIR)/a.out  # test files

check:
	scan-build make

clean: test_clean

docs:
	cd docs; \
	dox2html5 Doxyfile-mcss
