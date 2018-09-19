LIB=-L/usr/local/opt/flex/lib
LFLAGS=-lfl
FLEX=lex.yy.c
PROG=a.out
FLEX_SRC=comments.lex

.PHONY: all

$(PROG): $(FLEX)
	clang lex.yy.c $(LIB) $(LFLAGS) -O3 -std=c11 -Wall -Wpedantic -o $(PROG)

$(FLEX): $(FLEX_SRC)
	flex $(FLEX_SRC)

clean:
	rm -f $(PROG) $(FLEX)
