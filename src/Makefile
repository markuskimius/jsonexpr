CC:=clang
CPPFLAGS:=-g -O3

ifeq ($(findstring wasm,$(HOST)),wasm)
	PREFIX:=/opt/wasm
	BUILDDIR:=./build/wasm
	BINARY:=je.wasm
	ARCHIVE:=je.wasm

	EPREFIX:=$(PREFIX)
	BINDIR:=$(EPREFIX)/bin
	LIBDIR:=$(EPREFIX)/lib
	INCDIR:=$(PREFIX)/include

	LD:=wasm-ld
	CPPFLAGS:=-I$(INCDIR) --target=wasm32 -nostdlib $(CPPFLAGS)
	LDFLAGS:=-L$(LIBDIR) --export=je_parse --export=je_eval                                                                                                        \
		--export=je_nullval --export=je_boolval --export=je_intval --export=je_dblval --export=je_strval --export=je_arrval --export=je_objval --export=je_freeval \
		--export=je_gettype --export=je_getint64 --export=je_getdouble --export=je_getstring --export=je_getarray --export=je_getobject --export=je_getfunc        \
		--export=je_valtype --export=je_valtrue --export=je_valstr --export=je_valqstr --export=je_valcmp                                                          \
		--export=je_newtable --export=je_freetable --export=je_tableget --export=je_tableset --export=je_tableunset                                                \
		--export=je_newmap --export=je_freemap --export=je_mapget --export=je_mapset --export=je_mapunset                                                          \
		--export=je_mapnext --export=je_mapcmp --export=je_mapastr --export=je_mapkey --export=je_mapval                                                           \
		--export=je_newvec --export=je_freevec --export=je_vecset --export=je_vecget --export=je_vecpop --export=je_vecpush --export=je_vecunset                   \
		--export=je_vecback --export=je_veccmp --export=je_vecastr --export=je_veclen                                                                              \
		--export=je_freenode --export=je_nodetree
	LDLIBS:=-lc-wasm
else
	PREFIX:=..
	BUILDDIR:=./build/$(shell uname -m)
	BINARY:=je
	ARCHIVE:=libje.a

	EPREFIX:=$(PREFIX)
	BINDIR:=$(EPREFIX)/bin
	LIBDIR:=$(EPREFIX)/lib
	INCDIR:=$(PREFIX)/include/je

	CPPFLAGS:=-Wno-unknown-attributes $(CPPFLAGS)
	LDFLAGS:=-L$(BUILDDIR)
	LDLIBS:=-lm -lje
endif


##############################################################################
# PHONIES

.PHONY: all lib clean setup install uninstall wasm wasm-clean

all: setup $(BUILDDIR)/$(BINARY)

lib: setup $(BUILDDIR)/libje.a

setup:
	install -m 755 -d $(BUILDDIR)

clean:
	$(RM) $(BUILDDIR)/$(BINARY) $(BUILDDIR)/$(ARCHIVE) $(BUILDDIR)/*.o *.output *.tab.c *.tab.h *.je_yy.c je.h
	rmdir -p $(BUILDDIR) 2>/dev/null || true

install: all
	install -m 755 -d $(BINDIR)
	install -m 755 -d $(LIBDIR)
	install -m 755 -d $(INCDIR)
	install -m 755 $(BUILDDIR)/$(BINARY) $(BINDIR)
	install -m 644 $(BUILDDIR)/$(ARCHIVE) $(LIBDIR)
	@for i in *.h; do                         \
		echo install -m 644 $$i $(INCDIR);    \
		install -m 644 $$i $(INCDIR);         \
	done

uninstall:
	@for i in *.h; do                         \
		echo $(RM) $(INCDIR)/$$i;             \
		$(RM) $(INCDIR)/$$i;                  \
	done
	$(RM) $(BINDIR)/$(BINARY)
	$(RM) $(LIBDIR)/$(ARCHIVE)
	rmdir -p $(BINDIR) 2>/dev/null || true
	rmdir -p $(INCDIR) 2>/dev/null || true
	rmdir -p $(LIBDIR) 2>/dev/null || true

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

$(BUILDDIR)/je: $(BUILDDIR)/main.o $(BUILDDIR)/libje.a
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(BUILDDIR)/libje.a: $(BUILDDIR)/je_builtin.o $(BUILDDIR)/je_error.o $(BUILDDIR)/je_eval.o $(BUILDDIR)/je_func.o                            \
		$(BUILDDIR)/je_map.o $(BUILDDIR)/je_node.o $(BUILDDIR)/je_oper.o $(BUILDDIR)/je_symtbl.o $(BUILDDIR)/je_util.o $(BUILDDIR)/je_val.o \
		$(BUILDDIR)/je_vec.o $(BUILDDIR)/je_parse.tab.o $(BUILDDIR)/lex.je_yy.o
	llvm-ar rcs $@ $^

$(BUILDDIR)/je.wasm: $(BUILDDIR)/je_builtin.o $(BUILDDIR)/je_error.o $(BUILDDIR)/je_eval.o $(BUILDDIR)/je_func.o $(BUILDDIR)/main-wasm.o    \
		$(BUILDDIR)/je_map.o $(BUILDDIR)/je_node.o $(BUILDDIR)/je_oper.o $(BUILDDIR)/je_symtbl.o $(BUILDDIR)/je_util.o $(BUILDDIR)/je_val.o \
		$(BUILDDIR)/je_vec.o $(BUILDDIR)/je_parse.tab.o $(BUILDDIR)/lex.je_yy.o
	$(LD) $(LDFLAGS) $(TARGET_ARCH) $^ $(LDLIBS) -o $@ && chmod a-x $@

$(BUILDDIR)/main.o: main.c je.h

$(BUILDDIR)/main-wasm.o: main-wasm.c je_eval.h je_node.h je_parse.h je_val.h

$(BUILDDIR)/je_builtin.o: je_builtin.c je_builtin.h je_error.h je_eval.h je_func.h je_map.h je_val.h je_vec.h je_node.h je_symtbl.h je_util.h je.h

$(BUILDDIR)/je_error.o: je_error.c je_error.h je_node.h je_util.h

$(BUILDDIR)/je_eval.o: je_eval.c je_builtin.h je_error.h je_eval.h je_func.h je_map.h je_node.h je_oper.h je_symtbl.h je_val.h je_vec.h

$(BUILDDIR)/je_func.o: je_func.c je_error.h je_eval.h je_func.h je_symtbl.h je_val.h je_vec.h je_node.h je_map.h je_util.h

$(BUILDDIR)/je_map.o: je_map.c je_map.h je_util.h je_val.h

$(BUILDDIR)/je_node.o: je_node.c je_node.h je_util.h

$(BUILDDIR)/je_oper.o: je_oper.c je_eval.h je_oper.h je_val.h

$(BUILDDIR)/je_symtbl.o: je_symtbl.c je_builtin.h je_map.h je_symtbl.h je_val.h

$(BUILDDIR)/je_util.o: je_util.c je_parse.h je_util.h

$(BUILDDIR)/je_val.o: je_val.c je_error.h je_func.h je_map.h je_node.h je_util.h je_val.h je_vec.h

$(BUILDDIR)/je_vec.o: je_vec.c je_error.h je_util.h je_val.h je_vec.h

$(BUILDDIR)/je_parse.tab.o: je_parse.tab.c je_parse.tab.h je_node.h je_parse.h je_error.h je_val.h

$(BUILDDIR)/lex.je_yy.o: lex.je_yy.c je_parse.h je_error.h je_util.h

je_parse.tab.c je_parse.tab.h: je_parse.y
	bison -Dapi.prefix={je_yy} -t -v -d $<

lex.je_yy.c: je_parse.l
	flex -P je_yy $<

je_parse.h: je_parse.tab.h
	touch $@

je_node.h: je_parse.h
	touch $@

je.h: je.h.shdoc je_eval.h je_node.h je_parse.h je_val.h
	make -C .. preprocess

# vim:noet:
