CC:=clang
CPPFLAGS:=-g

ifeq ($(findstring wasm,$(TARGET)),wasm)
	LD:=wasm-ld
	PREFIX:=/opt/wasm
	CPPFLAGS:=-I$(PREFIX)/include --target=wasm32 -nostdlib $(CPPFLAGS)
	LDFLAGS:=-L$(PREFIX)/lib --export=newtable --export=freetable --export=parse --export=eval --export=valstr \
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

je.wasm: builtin.o error.o eval.o func.o main.o map.o node.o oper.o symtbl.o util.o val.o vec.o parse.tab.o lex.yy.o
	$(LD) $(LDFLAGS) $(TARGET_ARCH) $^ $(LDLIBS) -o $@ && chmod a-x $@

je: builtin.o error.o eval.o func.o main.o map.o node.o oper.o symtbl.o util.o val.o vec.o parse.tab.o lex.yy.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

builtin.o: builtin.c builtin.h error.h eval.h func.h map.h val.h vec.h

error.o: error.c error.h node.h util.h

eval.o: eval.c error.h eval.h func.h map.h node.h oper.h symtbl.h val.h vec.h

func.o: func.c error.h eval.h func.h symtbl.h val.h vec.h

main.o: main.c eval.h node.h parse.h val.h

map.o: map.c map.h util.h val.h

node.o: node.c node.h util.h

oper.o: oper.c eval.h oper.h val.h

symtbl.o: symtbl.c builtin.h map.h symtbl.h val.h

util.o: util.c parse.h util.h

val.o: val.c error.h func.h map.h util.h val.h vec.h

vec.o: vec.c error.h util.h val.h vec.h

parse.tab.o: parse.tab.c parse.tab.h node.h parse.h error.h val.h

lex.yy.o: lex.yy.c parse.h error.h util.h

parse.tab.c parse.tab.h: parse.y
	bison -t -v -d $<

lex.yy.c: parse.l
	flex $<

parse.h: parse.tab.h
	touch $@

node.h: parse.h
	touch $@


# vim:noet:
