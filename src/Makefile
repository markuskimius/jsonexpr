CC:=clang
CPPFLAGS:=-g -O3

ifeq ($(findstring wasm,$(HOST)),wasm)
	PREFIX:=/opt/wasm
	BUILDDIR:=./build/wasm
	BINARY:=$(BUILDDIR)/je.wasm
	MODE:=644

	EPREFIX:=$(PREFIX)
	BINDIR:=$(EPREFIX)/bin
	LIBDIR:=$(EPREFIX)/lib
	INCLUDEDIR:=$(PREFIX)/include

	LD:=wasm-ld
	CPPFLAGS:=-I$(INCLUDEDIR) --target=wasm32 -nostdlib $(CPPFLAGS)
	LDFLAGS:=-L$(LIBDIR) --export=je_parse --export=je_eval                                                                                                        \
		--export=je_nullval --export=je_boolval --export=je_intval --export=je_dblval --export=je_strval --export=je_arrval --export=je_objval --export=je_freeval \
		--export=je_gettype --export=je_getint64 --export=je_getdouble --export=je_getstring --export=je_getarray --export=je_getobject --export=je_getfunc        \
		--export=je_valtype --export=je_valtrue --export=je_valstr --export=je_valqstr --export=je_valcmp                                                          \
		--export=je_newtable --export=je_freetable --export=je_tableget --export=je_tableset --export=je_tableunset                                                \
		--export=je_newmap --export=je_freemap --export=je_mapget --export=je_mapset --export=je_mapunset                                                          \
		--export=je_mapnext --export=je_mapcmp --export=je_mapastr                                                                                                 \
		--export=je_newvec --export=je_freevec --export=je_vecset --export=je_vecget --export=je_vecpop --export=je_vecpush --export=je_vecback --export=je_veccmp --export=je_vecastr \
		--export=je_freenode --export=je_nodetree
	LDLIBS:=-lc-wasm
else
	PREFIX:=..
	BUILDDIR:=.
	BINARY:=$(BUILDDIR)/je
	MODE:=755

	EPREFIX:=$(PREFIX)
	BINDIR:=$(EPREFIX)/bin
	LIBDIR:=$(EPREFIX)/lib
	INCLUDEDIR:=$(PREFIX)/include

	CPPFLAGS:=-Wno-unknown-attributes $(CPPFLAGS)
	LDFLAGS:=-L$(BUILDDIR)
	LDLIBS:=-lm -lje
endif


##############################################################################
# PHONIES

.PHONY: all clean setup install uninstall wasm wasm-clean

all: setup $(BINARY)

setup:
	install -m 755 -d $(BUILDDIR)

clean:
	$(RM) $(BINARY) $(BUILDDIR)/libje.a $(BUILDDIR)/*.o *.output *.tab.c *.tab.h *.yy.c
	rmdir -p $(BUILDDIR) || true

install: all
	install -m 755 -d $(BINDIR)
	install -m $(MODE) $(BINARY) $(BINDIR)

uninstall:
	$(RM) $(BINDIR)/$(BINARY)
	rmdir -p $(BINDIR) || true

wasm:
	make HOST=wasm all

wasm-clean:
	make HOST=wasm clean


##############################################################################
# RULES

$(BUILDDIR)/%.o: %.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<


##############################################################################
# MAIN

$(BUILDDIR)/je: $(BUILDDIR)/libje.a
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(BUILDDIR)/je.wasm: $(BUILDDIR)/je_builtin.o $(BUILDDIR)/je_error.o $(BUILDDIR)/je_eval.o $(BUILDDIR)/je_func.o $(BUILDDIR)/je_main.o      \
		$(BUILDDIR)/je_map.o $(BUILDDIR)/je_node.o $(BUILDDIR)/je_oper.o $(BUILDDIR)/je_symtbl.o $(BUILDDIR)/je_util.o $(BUILDDIR)/je_val.o \
		$(BUILDDIR)/je_vec.o $(BUILDDIR)/je_parse.tab.o $(BUILDDIR)/lex.yy.o
	$(LD) $(LDFLAGS) $(TARGET_ARCH) $^ $(LDLIBS) -o $@ && chmod a-x $@

$(BUILDDIR)/libje.a: $(BUILDDIR)/je_builtin.o $(BUILDDIR)/je_error.o $(BUILDDIR)/je_eval.o $(BUILDDIR)/je_func.o $(BUILDDIR)/je_main.o      \
		$(BUILDDIR)/je_map.o $(BUILDDIR)/je_node.o $(BUILDDIR)/je_oper.o $(BUILDDIR)/je_symtbl.o $(BUILDDIR)/je_util.o $(BUILDDIR)/je_val.o \
		$(BUILDDIR)/je_vec.o $(BUILDDIR)/je_parse.tab.o $(BUILDDIR)/lex.yy.o
	llvm-ar rcs $@ $^

$(BUILDDIR)/je_builtin.o: je_builtin.c je_builtin.h je_error.h je_eval.h je_func.h je_map.h je_val.h je_vec.h je_node.h je_symtbl.h je_util.h

$(BUILDDIR)/je_error.o: je_error.c je_error.h je_node.h je_util.h

$(BUILDDIR)/je_eval.o: je_eval.c je_builtin.h je_error.h je_eval.h je_func.h je_map.h je_node.h je_oper.h je_symtbl.h je_val.h je_vec.h

$(BUILDDIR)/je_func.o: je_func.c je_error.h je_eval.h je_func.h je_symtbl.h je_val.h je_vec.h je_node.h je_map.h

$(BUILDDIR)/je_main.o: je_main.c je_eval.h je_node.h je_parse.h je_val.h

$(BUILDDIR)/je_map.o: je_map.c je_map.h je_util.h je_val.h

$(BUILDDIR)/je_node.o: je_node.c je_node.h je_util.h

$(BUILDDIR)/je_oper.o: je_oper.c je_eval.h je_oper.h je_val.h

$(BUILDDIR)/je_symtbl.o: je_symtbl.c je_builtin.h je_map.h je_symtbl.h je_val.h

$(BUILDDIR)/je_util.o: je_util.c je_parse.h je_util.h

$(BUILDDIR)/je_val.o: je_val.c je_error.h je_func.h je_map.h je_util.h je_val.h je_vec.h

$(BUILDDIR)/je_vec.o: je_vec.c je_error.h je_util.h je_val.h je_vec.h

$(BUILDDIR)/je_parse.tab.o: je_parse.tab.c je_parse.tab.h je_node.h je_parse.h je_error.h je_val.h

$(BUILDDIR)/je_lex.yy.o: lex.yy.c je_parse.h je_error.h je_util.h

je_parse.tab.c je_parse.tab.h: je_parse.y
	bison -t -v -d $<

lex.yy.c: je_parse.l
	flex $<

je_parse.h: je_parse.tab.h
	touch $@

je_node.h: je_parse.h
	touch $@


# vim:noet:
