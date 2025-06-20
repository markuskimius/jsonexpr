CC:=clang
ARCH:=$(shell uname -m)
BINARIES:=example
CPPFLAGS:=-I../../build/$(ARCH)/include
LDFLAGS:=-L../../build/$(ARCH)/lib
LDLIBS:=-lje


##############################################################################
# PHONIES

.PHONY: all clean

all: $(BINARIES)

clean:
	$(RM) $(BINARIES) *.o

example: example.o

example.o: example.c


# vim:noet:
