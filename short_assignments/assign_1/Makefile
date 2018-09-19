LIB="-L/usr/local/opt/flex/lib"
LFLAGS="-lfl"
FLEX=lex.yy.c
PROG=a.out

.PHONY: all

$(PROG): $(FLEX)
	clang lex.yy.c $(LIB) $(LFLAGS) -O3 -o $(PROG)

$(FLEX):
	flex comments.lex

clean:
	rm -f $(PROG) $(FLEX)
