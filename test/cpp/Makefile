CXX:=clang++
ARCH:=$(shell uname -m)
BINARIES:=getset example iterator
CPPFLAGS:=-I../../build/$(ARCH)/include
LDFLAGS:=-L../../build/$(ARCH)/lib
LDLIBS:=-ljepp -lstdc++


##############################################################################
# PHONIES

.PHONY: all clean

all: $(BINARIES)

clean:
	$(RM) $(BINARIES) *.o

getset: getset.o

getset.o: getset.cpp

example: example.o

example.o: example.cpp

iterator: iterator.o

iterator.o: iterator.cpp


# vim:noet:
