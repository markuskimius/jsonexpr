BINARIES=jsonexpr.wasm
PREFIX=/opt/wasm
CC=clang
LD=wasm-ld
CPPFLAGS=-I$(PREFIX)/include --target=wasm32 -nostdlib
# LDFLAGS=-L$(PREFIX)/lib --export=parse --export=eval --export=gettype --export=getint --export=getdouble --export=getstring --export=newsymtable
LDFLAGS=-L$(PREFIX)/lib --export-all
LDLIBS=-lc-wasm


##############################################################################
# PHONIES

.PHONY: all clean

all: $(BINARIES)

clean:
	$(RM) $(BINARIES) *.o *.output *.tab.c *.tab.h *.yy.c


##############################################################################
# MAIN

jsonexpr.wasm: value.o ast.o map.o ops.o eval.o util.o func.o ufunc.o vector.o nvector.o symtable.o builtins.o jsonexpr.tab.o lex.yy.o
	$(LD) $(LDFLAGS) $(TARGET_ARCH) $^ $(LDLIBS) -o $@ && chmod a-x $@

value.o: value.c value.h func.h ufunc.h map.h util.h vector.h

ast.o: ast.c ast.h

map.o: map.c map.h util.h vector.h

ops.o: ops.c ops.h ast.h map.h util.h vector.h value.h jsonexpr.tab.h

eval.o: eval.c eval.h ast.h ops.h map.h func.h ufunc.h value.h vector.h nvector.h symtable.h

func.o: func.c func.h ast.h value.h symtable.h

ufunc.o: ufunc.c ufunc.h ast.h eval.h value.h symtable.h

vector.o: vector.c vector.h util.h

nvector.o: nvector.c nvector.h util.h

symtable.o: symtable.c symtable.h map.h

builtins.o: builtins.c builtins.h ast.h map.h eval.h func.h ufunc.h value.h

jsonexpr.tab.o: jsonexpr.tab.c jsonexpr.tab.h ast.h eval.h map.h util.h value.h vector.h

lex.yy.o: lex.yy.c

jsonexpr.tab.c jsonexpr.tab.h: jsonexpr.y
	bison -t -v -d -Wcounterexamples $<

lex.yy.c: jsonexpr.l jsonexpr.tab.h util.h ast.h
	flex $<


# vim:noet:
