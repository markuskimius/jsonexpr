CC:=clang
BINARIES:=test
CFLAGS:=-I../../include
LDFLAGS:=-L../../lib
LDLIBS:=-lje


##############################################################################
# PHONIES

.PHONY: all clean

all: test

clean:
	$(RM) $(BINARIES) *.o


##############################################################################
# FILES

test: test.o

test.o: test.c


# vim:noet:
