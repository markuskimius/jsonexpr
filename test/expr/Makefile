.PHONY: all clean test test-arch test-wasm

all:
	@echo "Nothing to do."

clean:
	$(RM) -r var/diff var/output

test: test-arch test-wasm

test-arch:
	@./test-all.sh -a

test-wasm:
	@./test-all.sh -w

# vim:noet:
