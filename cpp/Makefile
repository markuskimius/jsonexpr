ARCHIVE:=libjepp.a
ARCH:=$(shell uname -m)
CXX:=clang++
CXXFLAGS:=-I../build/$(ARCH)/include
LDFLAGS:=-L.
PREFIX:=../build/$(ARCH)
EPREFIX:=$(PREFIX)
LIBDIR:=$(EPREFIX)/lib
INCDIR:=$(PREFIX)/include/jepp


##############################################################################
# PHONIES

.PHONY: all clean install

all: $(ARCHIVE)

clean:
	$(RM) $(ARCHIVE) *.o

install: $(ARCHIVE)
	install -m 755 -d $(LIBDIR)
	install -m 755 -d $(INCDIR)
	install -m 644 $(ARCHIVE) $(LIBDIR)
	@for i in *.h; do                          \
		echo install -m 644 $$i $(INCDIR);     \
		install -m 644 $$i $(INCDIR);          \
	done

uninstall:
	$(RM) $(LIBDIR)/$(ARCHIVE)
	@for i in *.h; do             \
		echo $(RM) $(INCDIR)/$$i; \
		$(RM) $(INCDIR)/$$i;      \
	done
	rmdir -p $(INCDIR) 2>/dev/null || true
	rmdir -p $(LIBDIR) 2>/dev/null || true


##############################################################################
# LIBRARY

libjepp.a: jepp.o jepp_error.o jepp_instance.o jepp_iterator.o jepp_parsed.o \
		jepp_symmap.o jepp_value.o
	cp -a $(LIBDIR)/libje.a $@
	llvm-ar rcs $@ $^


##############################################################################
# SOURCES

jepp.o: jepp.cpp jepp.h

jepp_error.o: jepp_error.cpp jepp_error.h

jepp_instance.o: jepp_instance.cpp jepp_instance.h

jepp_iterator.o: jepp_iterator.cpp jepp_error.h jepp_iterator.h jepp_value.h

jepp_parsed.o: jepp_parsed.cpp jepp_error.h jepp_parsed.h

jepp_symmap.o: jepp_symmap.cpp jepp_error.h jepp_symmap.h

jepp_value.o: jepp_value.cpp jepp_error.h jepp_iterator.h jepp_value.h


##############################################################################
# HEADERS

jepp.h: jepp_error.h jepp_instance.h jepp_iterator.h jepp_parsed.h \
		jepp_symmap.h jepp_value.h
	touch $@

jepp_instance.h: jepp_parsed.h jepp_symmap.h
	touch $@

jepp_parsed.h: jepp_symmap.h jepp_value.h
	touch $@

jepp_symmap.h: jepp_value.h
	touch $@


# vim:noet:
