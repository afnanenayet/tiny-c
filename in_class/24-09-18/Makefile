LIB=-L/usr/local/opt/flex/lib -L/usr/local/opt/llvm/lib -Wl,-rpath,/usr/local/opt/llvm/lib
LFLAGS=-lfl
PROG=a.out
LEX=flex
YACC=bison

.PHONY: all

$(PROG): calc2.l lex.yy.c calc2.tab.c
	clang $^ $(LIB) $(LFLAGS) -O3 -std=c11 -Wall -Wpedantic -o $(PROG)

lex.yy.c: calc2.l calc2.tab.h
	$(LEX) calc2.l

calc2.tab.h: calc2.y
	$(YACC) -d $^

calc2.tab.c: calc2.y
	$(YACC) -d $^

clean:
	rm -f lex.yy.c a.out calc2.tab.h calc2.tab.c
