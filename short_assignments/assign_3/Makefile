LIB=-L/usr/local/opt/flex/lib -L/usr/local/opt/llvm/lib -Wl,-rpath,/usr/local/opt/llvm/lib
LFLAGS=-lfl
PROG=a.out
LEX=flex
YACC=bison
LEX_SRC=calc.lex
YACC_SRC=calc.y

.PHONY: all

$(PROG): lex.yy.c calc.tab.c
	${CC} $^ $(LIB) $(LFLAGS) -O3 -std=c11 -Wall -Wpedantic -o $(PROG)

lex.yy.c: $(LEX_SRC) calc.tab.h
	$(LEX) $(LEX_SRC)

calc.tab.h: $(YACC_SRC)
	$(YACC) -d $^

calc.tab.c: $(YACC_SRC)
	$(YACC) -d $^

clean:
	rm -f lex.yy.c a.out calc.tab.h calc.tab.c
