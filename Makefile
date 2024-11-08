JE_VERSION:=0.0.15
SHDOC:=src/je.h src/main.c js/lib/je.js py/bin/je py/setup.py py/lib/je.py py/lib/__init__.py

.PHONY: all clean install uninstall preprocess

all: $(SHDOC)
	make -C src all
	make -C src HOST=wasm all

clean:
	make -C src clean
	make -C src HOST=wasm clean
	make -C py clean
	$(RM) $(SHDOC)

install: all
	make -C src install
	install -m 644 src/build/wasm/je.wasm js/lib
	install -m 644 src/build/wasm/je.wasm py/lib

uninstall:
	make -C src uninstall
	rm -f js/lib/je.wasm
	rm -f py/lib/je.wasm

preprocess: $(SHDOC)

src/je.h: src/je.h.shdoc Makefile
	JE_VERSION="$(JE_VERSION)" tool/shdoc $<

src/main.c: src/main.c.shdoc Makefile
	JE_VERSION="$(JE_VERSION)" tool/shdoc $<

js/lib/je.js: js/lib/je.js.shdoc Makefile
	JE_VERSION="$(JE_VERSION)" tool/shdoc $<

py/bin/je: py/bin/je.shdoc Makefile
	JE_VERSION="$(JE_VERSION)" tool/shdoc $< && chmod a+x $@

py/setup.py: py/setup.py.shdoc Makefile
	JE_VERSION="$(JE_VERSION)" tool/shdoc $< && chmod a+x $@

py/lib/je.py: py/lib/je.py.shdoc Makefile
	JE_VERSION="$(JE_VERSION)" tool/shdoc $<

py/lib/__init__.py: py/lib/__init__.py.shdoc Makefile
	JE_VERSION="$(JE_VERSION)" tool/shdoc $<


# vim:noet:
