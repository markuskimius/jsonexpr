.PHONY: all clean install uninstall

all:
	make -C src all
	make -C src HOST=wasm all

clean:
	make -C src clean
	make -C src HOST=wasm clean

install: all
	make -C src install
	install -m 644 src/build/wasm/je.wasm js/lib
	install -m 644 src/build/wasm/je.wasm py/lib

uninstall:
	make -C src uninstall
	rm -f js/lib/je.wasm
	rm -f py/lib/je.wasm

# vim:noet:
