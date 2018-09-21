LIB=-L/usr/local/opt/flex/lib -L/usr/local/opt/llvm/lib -Wl,-rpath,/usr/local/opt/llvm/lib
LFLAGS=-lfl
FLEX=lex.yy.c
PROG=a.out
FLEX_SRC=counter.lex

.PHONY: all

$(PROG): $(FLEX)
	clang lex.yy.c $(LIB) $(LFLAGS) -O3 -std=c11 -Wall -Wpedantic -o $(PROG)

$(FLEX): $(FLEX_SRC)
	flex $(FLEX_SRC)

clean:
	rm -f $(PROG) $(FLEX)
