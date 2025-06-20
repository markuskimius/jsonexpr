CC:=clang
CPPFLAGS:=-g -O0
BINARIES:=basic

ifeq ($(findstring wasm,$(HOST)),wasm)
	PREFIX:=../../build/wasm
	EPREFIX:=$(PREFIX)
	BINDIR:=$(EPREFIX)/bin
	LIBDIR:=$(EPREFIX)/lib
	INCDIR:=$(PREFIX)/include/je
	BUILDDIR:=./build/wasm

	LD:=wasm-ld
	CPPFLAGS:=-I$(INCDIR) $(CPPFLAGS) -I/opt/wasm/include --target=wasm32 -nostdlib
	LDFLAGS:=-L$(LIBDIR) -L/opt/wasm/lib
	LDLIBS:=-lje -lc-wasm
	EXT:=.wasm
else
	PREFIX:=../../build/$(shell uname -m)
	EPREFIX:=$(PREFIX)
	BINDIR:=$(EPREFIX)/bin
	LIBDIR:=$(EPREFIX)/lib
	INCDIR:=$(PREFIX)/include/je
	BUILDDIR:=./build/$(shell uname -m)

	CPPFLAGS:=-I$(INCDIR) $(CPPFLAGS)
	LDFLAGS:=-L$(LIBDIR)
	LDLIBS:=-lm -lje
endif


##############################################################################
# PHONIES

.PHONY: all clean arch arch-clean wasm wasm-clean

all: arch wasm

clean: arch-clean wasm-clean

arch:
	install -m 755 -d $(BUILDDIR)
	@for bin in $(BINARIES); do         \
		make $(BUILDDIR)/$${bin}$(EXT); \
	done

arch-clean:
	$(RM) -r output
	$(RM) -r $(BUILDDIR)
	rmdir -p build 2>/dev/null || true

wasm:
	make HOST=wasm arch

wasm-clean:
	make HOST=wasm arch-clean

test:
	@./test-all.sh


##############################################################################
# RULES

$(BUILDDIR)/%.o: %.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<

%.wasm: %.o
	$(LD) $(LDFLAGS) $(TARGET_ARCH) $< $(LDLIBS) -o $@ && chmod a-x $@


##############################################################################
# FILES

$(BUILDDIR)/basic$(SUFFIX): $(BUILDDIR)/basic.o

$(BUILDDIR)/basic.o: basic.c


# vim:noet:
