JE_VERSION:=0.0.14
SHDOC_FILES:=src/je.h py/setup.py py/lib/__init__.py py/lib/je.py

.PHONY: all clean install uninstall preprocess

all: preprocess
	make -C src all
	make -C src HOST=wasm all

clean:
	make -C src clean
	make -C src HOST=wasm clean
	$(RM) $(SHDOC_FILES)

install: all
	make -C src install
	install -m 644 src/build/wasm/je.wasm js/lib
	install -m 644 src/build/wasm/je.wasm py/lib

uninstall:
	make -C src uninstall
	rm -f js/lib/je.wasm
	rm -f py/lib/je.wasm

preprocess:
	@for shdoc in $(SHDOC_FILES); do                            \
		JE_VERSION="$(JE_VERSION)" tool/shdoc "$$shdoc.shdoc";  \
	done

# vim:noet:
