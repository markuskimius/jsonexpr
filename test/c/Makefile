CC:=clang
BINARIES:=simple
CFLAGS:=-I../../include -g
LDFLAGS:=-L../../lib -g
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


# vim:noet:
