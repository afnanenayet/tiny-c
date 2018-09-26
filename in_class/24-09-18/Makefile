LIB=-L/usr/local/opt/flex/lib -L/usr/local/opt/llvm/lib -Wl,-rpath,/usr/local/opt/llvm/lib
LFLAGS=-lfl
PROG=a.out
LEX=flex
YACC=bison
SRC=calc1


.PHONY: all

$(PROG): lex.yy.c $(SRC).tab.c
	clang $^ $(LIB) $(LFLAGS) -O3 -std=c11 -Wall -Wpedantic -o $(PROG)

lex.yy.c: $(SRC).l $(SRC).tab.h
	$(LEX) calc2.l

$(SRC).tab.h: $(SRC).y
	$(YACC) -d $^

$(SRC).tab.c: $(SRC).y
	$(YACC) -d $^

clean:
	rm -f lex.yy.c a.out $(SRC).tab.h $(SRC).tab.c
