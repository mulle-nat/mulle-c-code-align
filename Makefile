#
# old skool Makefile by Nat!
#
DST=/usr/local/bin

all:	mulle-variable-align

mulle-variable-align:		main.c
	cc -o mulle-variable-align main.c

install:	all
	install mulle-variable-align $(DST)
	ln -f $(DST)/mulle-variable-align $(DST)/mulle-assignment-align

clean:
	- rm mulle-variable-align
