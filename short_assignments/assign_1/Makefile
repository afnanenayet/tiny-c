LIB="-L/usr/local/opt/flex/lib"
LFLAGS = "-lfl"

.PHONY: all

prog: lex
	clang lex.yy.c $(LIB) $(LFLAGS)

lex:
	flex comments.lex

clean:
	rm -f *.out lex.yy.c
