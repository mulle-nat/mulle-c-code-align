#
# old skool Makefile by Nat!
#
INSTALL_PATH=/usr/local/bin

all:	mulle-variable-align

mulle-variable-align:		main.c
	cc -o mulle-variable-align main.c

install:	all
	install mulle-variable-align $(INSTALL_PATH)
	ln -f $(INSTALL_PATH)/mulle-variable-align $(INSTALL_PATH)/mulle-assignment-align

clean:
	- rm mulle-variable-align
