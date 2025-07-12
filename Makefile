JE_VERSION:=0.2.5
SHDOC:=src/je_const.h
JSSHDOC:=js/lib/je.js
PYSHDOC:=py/setup.py py/bin/je py/lib/je.py py/lib/je_wasmer.py py/lib/je_wasmtime.py py/lib/__init__.py

ifeq ($(findstring wasm,$(HOST)),wasm)
	PREFIX:=./build/wasm
	EPREFIX:=$(PREFIX)
	BINDIR:=$(EPREFIX)/bin
	LIBDIR:=$(EPREFIX)/lib
	INCDIR:=$(PREFIX)/include/je

	ALL:=wasm
	CLEAN:=wasm-clean
	DISTCLEAN:=wasm-distclean
	BUILDDIR:=./build/wasm
else
	PREFIX:=./build/$(shell uname -m)
	EPREFIX:=$(PREFIX)
	BINDIR:=$(EPREFIX)/bin
	LIBDIR:=$(EPREFIX)/lib
	INCDIR:=$(PREFIX)/include/je

	ALL:=arch
	CLEAN:=arch-clean
	DISTCLEAN:=arch-distclean
	BUILDDIR:=./build/$(shell uname -m)
endif


##############################################################################
# PHONIES

.PHONY: all clean install uninstall
.PHONY: arch arch-clean arch-install arch-uninstall
.PHONY: wasm wasm-clean wasm-install wasm-uninstall
.PHONY: test test-clean

all: arch wasm

clean: arch-clean wasm-clean test-clean

distclean: arch-distclean wasm-distclean test-distclean

install: arch-install wasm-install

uninstall: arch-uninstall wasm-uninstall

arch: $(SHDOC)
	make -C src $(ALL)

arch-clean:
	make -C src $(CLEAN)
	$(RM) $(SHDOC)

arch-distclean:
	make -C src $(DISTCLEAN)

arch-install: $(ALL)
	install -m 755 -d $(INCDIR)
	install -m 755 -d $(LIBDIR)
	install -m 644 src/$(BUILDDIR)/libje.a $(LIBDIR)
	@for i in src/*.h; do                     \
		echo install -m 644 $$i $(INCDIR);    \
		install -m 644 $$i $(INCDIR);         \
	done
	@if [ "$(HOST)" == "wasm" ]; then                           \
		install -m 755 -d js/lib;                               \
		install -m 755 -d py/lib;                               \
		install -m 755 -d test/expr/lib;                        \
		install -m 644 src/build/wasm/je.wasm js/lib;           \
		install -m 644 src/build/wasm/je.wasm py/lib;           \
		install -m 644 src/build/wasm/je.wasm test/expr/lib;    \
	else                                                        \
		install -m 755 -d $(BINDIR);                            \
		install -m 755 src/$(BUILDDIR)/je $(BINDIR);            \
	fi

arch-uninstall:
	@if [ "$(HOST)" == "wasm" ]; then                 \
		$(RM) js/lib/je.wasm;                         \
		$(RM) py/lib/je.wasm;                         \
		$(RM) test/expr/lib/je.wasm;                  \
		rmdir -p js/lib 2>/dev/null || true;          \
		rmdir -p py/lib 2>/dev/null || true;          \
		rmdir -p test/expr/lib 2>/dev/null || true;   \
	else                                              \
		$(RM) $(BINDIR)/je;                           \
		rmdir -p $(BINDIR) 2>/dev/null || true;       \
	fi
	@for i in src/*.h; do                     \
		echo $(RM) $(INCDIR)/$${i##*/};       \
		$(RM) $(INCDIR)/$${i##*/};            \
	done
	$(RM) $(LIBDIR)/libje.a
	rmdir -p $(INCDIR) 2>/dev/null || true
	rmdir -p $(LIBDIR) 2>/dev/null || true

wasm: $(JSSHDOC) $(PYSHDOC)
	make HOST=wasm arch

wasm-clean:
	make HOST=wasm arch-clean
	$(RM) $(JSSHDOC) $(PYSHDOC)

wasm-distclean:
	make HOST=wasm arch-distclean

wasm-install:
	make HOST=wasm arch-install

wasm-uninstall:
	make HOST=wasm arch-uninstall

test: install
	make -C test/arch all
	make -C test/expr all

test-clean:
	make -C test/arch clean
	make -C test/expr clean

test-distclean: test-clean


##############################################################################
# FILES

src/je_const.h: src/je_const.h.shdoc Makefile
	$(RM) $@ && JE_VERSION="$(JE_VERSION)" buildtool/shdoc -o $@ $< && chmod 444 $@

js/lib/je.js: js/lib/je.js.shdoc Makefile
	$(RM) $@ && JE_VERSION="$(JE_VERSION)" buildtool/shdoc -o $@ $< && chmod 444 $@

py/setup.py: py/setup.py.shdoc Makefile
	$(RM) $@ && JE_VERSION="$(JE_VERSION)" buildtool/shdoc -o $@ $< && chmod 555 $@

py/bin/je: py/bin/je.shdoc Makefile
	$(RM) $@ && JE_VERSION="$(JE_VERSION)" buildtool/shdoc -o $@ $< && chmod 555 $@

py/lib/je.py: py/lib/je.py.shdoc Makefile
	$(RM) $@ && JE_VERSION="$(JE_VERSION)" buildtool/shdoc -o $@ $< && chmod 444 $@

py/lib/je_wasmer.py: py/lib/je_wasmer.py.shdoc Makefile
	$(RM) $@ && JE_VERSION="$(JE_VERSION)" buildtool/shdoc -o $@ $< && chmod 444 $@

py/lib/je_wasmtime.py: py/lib/je_wasmtime.py.shdoc Makefile
	$(RM) $@ && JE_VERSION="$(JE_VERSION)" buildtool/shdoc -o $@ $< && chmod 444 $@

py/lib/__init__.py: py/lib/__init__.py.shdoc Makefile
	$(RM) $@ && JE_VERSION="$(JE_VERSION)" buildtool/shdoc -o $@ $< && chmod 444 $@


# vim:noet:
