CC:=clang
BINARIES:=simple nav
CFLAGS:=-I../../include
LDFLAGS:=-L../../lib
LDLIBS:=-lje -lm


##############################################################################
# PHONIES

.PHONY: all clean

all: $(BINARIES)

clean:
	$(RM) $(BINARIES) *.o


##############################################################################
# FILES

simple: simple.o

simple.o: simple.c

nav: nav.o

nav.o: nav.c


# vim:noet:
