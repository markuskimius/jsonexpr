CC:=clang
CPPFLAGS:=-g

ifeq ($(findstring wasm,$(TARGET)),wasm)
	LD:=wasm-ld
	PREFIX:=/opt/wasm
	CPPFLAGS:=-I$(PREFIX)/include --target=wasm32 -nostdlib $(CPPFLAGS)
	LDFLAGS:=-L$(PREFIX)/lib --export=newtable --export=freetable --export=parse --export=eval --export=valuestr \
		--export=gettype --export=getint64 --export=getdouble --export=getstring --export=tableget
	LDLIBS:=-lc-wasm
else
	CPPFLAGS:=-Wno-unknown-attributes $(CPPFLAGS)
	LDLIBS:=-lm
endif


##############################################################################
# PHONIES

.PHONY: all c wasm clean

c:
	make TARGET:=c je

wasm:
	make TARGET=wasm je.wasm

clean:
	$(RM) je.wasm je *.o *.output *.tab.c *.tab.h *.yy.c


##############################################################################
# MAIN

je.wasm: builtin.o eval.o func.o main.o map.o node.o oper.o symtbl.o error.o util.o value.o vec.o parse.tab.o lex.yy.o
	$(LD) $(LDFLAGS) $(TARGET_ARCH) $^ $(LDLIBS) -o $@ && chmod a-x $@

je: builtin.o eval.o func.o main.o map.o node.o oper.o symtbl.o error.o util.o value.o vec.o parse.tab.o lex.yy.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

builtin.o: builtin.c builtin.h eval.h func.h map.h node.h value.h vec.h

eval.o: eval.c eval.h func.h map.h node.h oper.h error.h value.h vec.h symtbl.h

func.o: func.c eval.h func.h node.h value.h vec.h symtbl.h

main.o: main.c eval.h map.h util.h parse.h value.h vec.h

map.o: map.c map.h util.h value.h vec.h

node.o: node.c node.h

oper.o: oper.c map.h eval.h node.h oper.h util.h parse.h value.h vec.h

error.o: error.c node.h error.h

symtbl.o: symtbl.c map.h builtin.h symtbl.h value.h

util.o: util.c util.h

value.o: value.c func.h map.h util.h value.h vec.h

vec.o: vec.c util.h value.h vec.h

parse.tab.o: parse.tab.c parse.tab.h node.h parse.h error.h

lex.yy.o: lex.yy.c node.h parse.h error.h util.h

parse.tab.c parse.tab.h: parse.y
	bison -t -v -d $<

lex.yy.c: parse.l
	flex $<

parse.h: parse.tab.h
	touch $@

node.h: parse.h
	touch $@


# vim:noet:
