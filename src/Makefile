CC:=clang
CPPFLAGS:=-g -O0

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
	LDFLAGS:=-L$(LIBDIR) --export=je_parse --export=JE_EvalByNode                                                                                                        \
		--export=JE_ValNewFromNull --export=JE_ValNewFromBool --export=JE_ValNewFromInt --export=JE_ValNewFromFloat --export=JE_ValNewFromCstr                           \
		--export=JE_ValNewFromVec --export=JE_ValNewFromMap --export=JE_ValDelete                                                                                        \
		--export=JE_ValGetBool --export=JE_ValGetInt --export=JE_ValGetFloat --export=JE_ValGetString --export=JE_ValGetVec --export=JE_ValGetMap --export=JE_ValGetFunc \
		--export=JE_ValGetType --export=JE_ValIsTrue --export=JE_ValToCstr --export=JE_ValToQstr --export=JE_ValCmp                                                      \
		--export=je_newtable --export=je_freetable --export=je_tableget --export=je_tableset --export=je_tableunset --export=je_tableclear                               \
		--export=JE_MapNew --export=JE_MapDelete --export=JE_MapGet --export=JE_MapSet --export=JE_MapUnset --export=JE_MapClear                                         \
		--export=JE_MapNext --export=JE_MapCmp --export=JE_MapToAstr --export=JE_MapKey --export=JE_MapVal                                                               \
		--export=JE_VecNew --export=JE_VecDelete --export=JE_VecSet --export=JE_VecPop --export=JE_VecPush --export=JE_VecUnset --export=JE_VecGet --export=JE_VecBack   \
		--export=JE_VecCmp --export=JE_VecToAstr --export=JE_VecLen                                                                                                      \
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
		$(BUILDDIR)/je_vec.o $(BUILDDIR)/je_parse.tab.o $(BUILDDIR)/lex.je_yy.o $(BUILDDIR)/je_token.o $(BUILDDIR)/je.o
	llvm-ar rcs $@ $^

$(BUILDDIR)/je.wasm: $(BUILDDIR)/je_builtin.o $(BUILDDIR)/je_error.o $(BUILDDIR)/je_eval.o $(BUILDDIR)/je_func.o $(BUILDDIR)/main-wasm.o    \
		$(BUILDDIR)/je_map.o $(BUILDDIR)/je_node.o $(BUILDDIR)/je_oper.o $(BUILDDIR)/je_symtbl.o $(BUILDDIR)/je_util.o $(BUILDDIR)/je_val.o \
		$(BUILDDIR)/je_vec.o $(BUILDDIR)/je_parse.tab.o $(BUILDDIR)/lex.je_yy.o $(BUILDDIR)/je_token.o
	$(LD) $(LDFLAGS) $(TARGET_ARCH) $^ $(LDLIBS) -o $@ && chmod a-x $@

$(BUILDDIR)/main.o: main.c je.h

$(BUILDDIR)/je.o: je.c je.h je_map.h je_error.h je_util.h

$(BUILDDIR)/main-wasm.o: main-wasm.c je_eval.h je_node.h je_parse.h je_val.h

$(BUILDDIR)/je_builtin.o: je_builtin.c je_builtin.h je_error.h je_eval.h je_func.h je_map.h je_val.h je_vec.h je_node.h je_symtbl.h je_util.h je.h

$(BUILDDIR)/je_error.o: je_error.c je_error.h je_node.h je_util.h je_token.h

$(BUILDDIR)/je_eval.o: je_eval.c je_builtin.h je_error.h je_eval.h je_func.h je_map.h je_node.h je_oper.h je_symtbl.h je_val.h je_vec.h

$(BUILDDIR)/je_func.o: je_func.c je_error.h je_eval.h je_func.h je_symtbl.h je_val.h je_vec.h je_node.h je_map.h je_util.h

$(BUILDDIR)/je_map.o: je_map.c je_map.h je_util.h je_val.h

$(BUILDDIR)/je_node.o: je_node.c je_node.h je_util.h je_token.h

$(BUILDDIR)/je_token.o: je_token.c je_token.h je_util.h

$(BUILDDIR)/je_oper.o: je_oper.c je_eval.h je_oper.h je_val.h je_util.h

$(BUILDDIR)/je_symtbl.o: je_symtbl.c je_builtin.h je_map.h je_symtbl.h je_val.h je_util.h

$(BUILDDIR)/je_util.o: je_util.c je_parse.h je_util.h je_token.h

$(BUILDDIR)/je_val.o: je_val.c je_error.h je_func.h je_map.h je_node.h je_util.h je_val.h je_vec.h

$(BUILDDIR)/je_vec.o: je_vec.c je_error.h je_util.h je_val.h je_vec.h

$(BUILDDIR)/je_parse.tab.o: je_parse.tab.c je_parse.tab.h je_node.h je_parse.h je_error.h je_val.h

$(BUILDDIR)/lex.je_yy.o: lex.je_yy.c je_parse.h je_error.h je_util.h je_node.h

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
