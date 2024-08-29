.PHONY: all wasm clean

all:
	make -C src

wasm:
	make -C src wasm

clean:
	make -C src clean


# vim:noet:
