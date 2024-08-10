BINARIES=jsonexpr.wasm
PREFIX=/opt/wasm
CC=clang
LD=wasm-ld
CPPFLAGS=-I$(PREFIX)/include --target=wasm32 -nostdlib
LDFLAGS=-L$(PREFIX)/lib --export=newtable --export=freetable --export=parse --export=eval --export=strdecoded \
	--export=gettype --export=getint64 --export=getdouble --export=getstring --export=gettable
LDLIBS=-lc-wasm


##############################################################################
# PHONIES

.PHONY: all clean

all: $(BINARIES)

clean:
	$(RM) $(BINARIES) *.o *.output *.tab.c *.tab.h *.yy.c


##############################################################################
# MAIN

jsonexpr.wasm: builtin.o eval.o func.o main.o map.o node.o oper.o symtable.o throw.o token.o ufunc.o util.o value.o vector.o parse.tab.o lex.yy.o
	$(LD) $(LDFLAGS) $(TARGET_ARCH) $^ $(LDLIBS) -o $@ && chmod a-x $@

builtin.o: builtin.c builtin.h eval.h func.h map.h node.h ufunc.h value.h vector.h

eval.o: eval.c eval.h func.h map.h node.h oper.h throw.h token.h ufunc.h value.h vector.h symtable.h

func.o: func.c eval.h func.h node.h value.h vector.h symtable.h

main.o: main.c eval.h map.h util.h parse.h value.h vector.h

map.o: map.c map.h util.h value.h vector.h

node.o: node.c node.h token.h

oper.o: oper.c map.h node.h oper.h util.h parse.h value.h vector.h

throw.o: throw.c node.h throw.h token.h

token.o: token.c token.h

symtable.o: symtable.c map.h builtin.h symtable.h value.h

ufunc.o: ufunc.c eval.h func.h node.h util.h ufunc.h value.h vector.h symtable.h

util.o: util.c util.h

value.o: value.c func.h map.h util.h ufunc.h value.h vector.h

vector.o: vector.c util.h value.h vector.h

parse.tab.o: parse.tab.c parse.tab.h node.h parse.h throw.h token.h

lex.yy.o: lex.yy.c node.h parse.h throw.h token.h util.h

parse.tab.c parse.tab.h: parse.y
	bison -t -v -d -Wcounterexamples $<

lex.yy.c: parse.l
	flex $<

parse.h: parse.tab.h
	touch $@

token.h: parse.h
	touch $@

node.h: parse.h
	touch $@


# vim:noet:
