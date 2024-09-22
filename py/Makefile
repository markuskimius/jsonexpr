.PHONY: all clean install uninstall dist upload

all:
	make -C .. install

clean:
	rm -rf build dist lib/__pycache__ lib/je.wasm

install: all
	pip3 install .

uninstall:
	pip3 uninstall jsonexpr

dist: all
	./setup.py sdist

upload: dist
	python3 -m twine upload dist/*.tar.gz

# vim:noet
