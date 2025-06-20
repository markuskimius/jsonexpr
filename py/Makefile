.PHONY: all clean install uninstall dist upload

all:
	make -C .. install

clean:
	$(RM) -r build dist lib/__pycache__ lib/je.wasm src/*.egg-info

install: all
	pip install .

uninstall:
	pip uninstall jsonexpr

dist: all
	python3 setup.py sdist

upload: dist
	python3 -m twine upload dist/*.tar.gz

# vim:noet
