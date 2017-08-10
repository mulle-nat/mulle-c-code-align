#
# old skool Makefile by Nat!
#
ifndef INSTALL_PREFIX
INSTALL_PREFIX=/usr/local
endif

all:	mulle-variable-align

mulle-variable-align:		main.c
	cc -o mulle-variable-align main.c

test:
	true

install:	all
	mkdir -p $(INSTALL_PREFIX)/bin
	install mulle-variable-align $(INSTALL_PREFIX)/bin
	ln -f $(INSTALL_PREFIX)/bin/mulle-variable-align $(INSTALL_PREFIX)/bin/mulle-assignment-align

clean:
	- rm mulle-variable-align
