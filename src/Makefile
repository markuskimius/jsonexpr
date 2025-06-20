CC:=clang
# CPPFLAGS:=-g -O0
CPPFLAGS:=-O2
ARCHIVE:=libje.a

ifeq ($(findstring wasm,$(HOST)),wasm)
	BUILDDIR:=./build/wasm
	BINARY:=je.wasm

	PREFIX:=/opt/wasm
	EPREFIX:=$(PREFIX)
	LIBDIR:=$(EPREFIX)/lib
	INCDIR:=$(PREFIX)/include

	LD:=wasm-ld
	CPPFLAGS:=-I$(INCDIR) --target=wasm32 -nostdlib $(CPPFLAGS)
	LDFLAGS:=-L$(LIBDIR) -L$(BUILDDIR) -z stack-size=0x100000
	LDLIBS:=-lc-wasm -lje
else
	BUILDDIR:=./build/$(shell uname -m)
	BINARY:=je

	CPPFLAGS:=-Wno-unknown-attributes $(CPPFLAGS)
	LDFLAGS:=-L$(BUILDDIR)
	LDLIBS:=-lm -lje
endif


##############################################################################
# PHONIES

.PHONY: all clean setup arch arch-clean wasm wasm-clean

all: arch wasm

clean: arch-clean wasm-clean

distclean: arch-distclean wasm-distclean

setup:
	install -m 755 -d $(BUILDDIR)

arch: setup $(BUILDDIR)/$(BINARY)

arch-clean: arch-distclean
	$(RM) *.tab.c *.tab.h *.JE_yy.c

arch-distclean:
	$(RM) $(BUILDDIR)/$(BINARY) $(BUILDDIR)/$(ARCHIVE) $(BUILDDIR)/*.o *.output
	rmdir -p $(BUILDDIR) 2>/dev/null || true

wasm:
	make HOST=wasm arch

wasm-clean:
	make HOST=wasm arch-clean

wasm-distclean:
	make HOST=wasm arch-distclean


##############################################################################
# RULES

$(BUILDDIR)/%.o: %.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<


##############################################################################
# FILES

$(BUILDDIR)/je: $(BUILDDIR)/main.o $(BUILDDIR)/libje.a
	$(LINK.o) $< $(LOADLIBES) $(LDLIBS) -o $@

$(BUILDDIR)/je.wasm: $(BUILDDIR)/main-wasm.o $(BUILDDIR)/libje.a
	$(LD) $(LDFLAGS) $(TARGET_ARCH) $< $(LDLIBS) -o $@ && chmod a-x $@

$(BUILDDIR)/libje.a: $(BUILDDIR)/je_ast.o $(BUILDDIR)/je_astinfo.o             \
		$(BUILDDIR)/je_astnode.o $(BUILDDIR)/je_chr.o $(BUILDDIR)/je_error.o   \
		$(BUILDDIR)/je_fun.o $(BUILDDIR)/je_map.o $(BUILDDIR)/je_nod.o         \
		$(BUILDDIR)/je_node.o $(BUILDDIR)/je_ntype.o $(BUILDDIR)/je_ref.o      \
		$(BUILDDIR)/je_str.o $(BUILDDIR)/je_symbols.o $(BUILDDIR)/je_symmap.o  \
		$(BUILDDIR)/je_token.o $(BUILDDIR)/je_type.o $(BUILDDIR)/je_util.o     \
		$(BUILDDIR)/je_value.o $(BUILDDIR)/je_vec.o $(BUILDDIR)/je_parse.tab.o \
		$(BUILDDIR)/lex.JE_yy.o
	llvm-ar rcs $@ $^

$(BUILDDIR)/main.o: main.c je.h

$(BUILDDIR)/main-wasm.o: main-wasm.c je.h je_util.h

$(BUILDDIR)/je_ast.o: je_ast.c je_ast.h je_node.h je_ntype.h je_value.h

$(BUILDDIR)/je_astinfo.o: je_astinfo.c je_astinfo.h je_token.h je_util.h

$(BUILDDIR)/je_astnode.o: je_astnode.c je_astinfo.h je_astnode.h je_node.h je_ntype.h je_util.h

$(BUILDDIR)/je_chr.o: je_chr.c je_chr.h

$(BUILDDIR)/je_error.o: je_error.c je_ast.h je_error.h je_token.h je_util.h

$(BUILDDIR)/je_fun.o: je_fun.c je_error.h je_fun.h je_node.h je_ntype.h je_ref.h je_type.h je_util.h je_value.h je_vec.h

$(BUILDDIR)/je_map.o: je_map.c je_map.h je_str.h je_type.h je_util.h je_value.h

$(BUILDDIR)/je_nod.o: je_nod.c je_nod.h

$(BUILDDIR)/je_node.o: je_node.c je_astnode.h je_node.h je_ntype.h

$(BUILDDIR)/je_ntype.o: je_ntype.c je_ast.h je_error.h je_map.h je_node.h je_ntype.h je_symmap.h je_type.h je_token.h je_util.h je_vec.h

$(BUILDDIR)/je_ref.o: je_ref.h je_util.h

$(BUILDDIR)/je_str.o: je_str.c je_chr.h je_str.h je_util.h

$(BUILDDIR)/je_symmap.o: je_symmap.c je_map.h je_ref.h je_str.h je_symmap.h je_type.h je_util.h je_value.h

$(BUILDDIR)/je_symbols.o: je_symbols.c je_const.h je_error.h je_map.h je_node.h je_ntype.h je_ref.h je_symbols.h je_symmap.h je_type.h je_util.h je_value.h je_vec.h

$(BUILDDIR)/je_token.o: je_token.c je_token.h

$(BUILDDIR)/je_type.o: je_type.c je_error.h je_node.h je_ntype.h je_ref.h je_token.h je_type.h je_util.h je_value.h

$(BUILDDIR)/je_util.o: je_util.c je_str.h je_util.h

$(BUILDDIR)/je_value.o: je_value.c je_ref.h je_type.h je_util.h je_value.h

$(BUILDDIR)/je_vec.o: je_vec.c je_type.h je_util.h je_value.h je_vec.h

$(BUILDDIR)/je_parse.tab.o: je_parse.tab.c

$(BUILDDIR)/lex.JE_yy.o: lex.JE_yy.c

lex.JE_yy.c: je_parse.l je_ast.h je_chr.h je_error.h je_parse.h je_parse.tab.h je_token.h je_util.h je_value.h
	flex -P JE_yy $<

je_parse.tab.c je_parse.tab.h: je_parse.y je_ast.h je_error.h je_node.h je_parse.h je_token.h je_ast.h je_str.h je_token.h je_value.h
	bison -Dapi.prefix={JE_yy} -t -v -d -Wcounterexamples $<

je.h: je_ast.h je_const.h je_parse.h je_symbols.h je_symmap.h je_type.h je_value.h
	touch $@

je_ast.h: je_astnode.h
	touch $@

je_astnode.h: je_astinfo.h
	touch $@

je_node.h: je_parse.h je_value.h
	touch $@

je_ref.h: je_fun.h je_map.h je_vec.h
	touch $@

je_symmap.h: je_map.h
	touch $@

je_token.h: je_parse.h je_str.h
	touch $@

je_value.h: je_fun.h je_nod.h je_str.h
	touch $@


# vim:noet:
