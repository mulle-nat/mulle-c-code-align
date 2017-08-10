#
# old skool Makefile by Nat!
#
ifndef INSTALL_PATH
INSTALL_PATH=/usr/local/bin
endif

all:	mulle-variable-align

mulle-variable-align:		main.c
	cc -o mulle-variable-align main.c

test:
	true

install:	all
	install -d mulle-variable-align $(INSTALL_PATH)
	ln -f $(INSTALL_PATH)/mulle-variable-align $(INSTALL_PATH)/mulle-assignment-align

clean:
	- rm mulle-variable-align
