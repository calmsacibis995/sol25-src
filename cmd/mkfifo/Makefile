#
#ident	"@(#)Makefile	1.5	95/02/03 SMI"
#
# Copyright (c) 1990 by Sun Microsystems, Inc.
#
# cmd/mkfifo/Makefile
#

PROG= mkfifo
OBJS= $(PROG).o common.o
SRCS= $(OBJS:%.o=%.c)

include ../Makefile.cmd

CFLAGS += -v
LDLIBS += -lintl

%.o: ../chmod/%.c
	$(COMPILE.c) -o $@ $<

#  The following was derived from the default .c.po rule in the master
#  makefile.  It had to be adapted to avoid writing the intermediate (.i)
#  file in ../chmod.

%.po: ../chmod/%.c
	$(COMPILE.cpp) $< > $*.c.i
	$(XGETTEXT) $(XGETFLAGS) $*.c.i ;\
	$(RM)	$@ ;\
	sed "/^domain/d" < messages.po  > $@ ;\
	$(RM) messages.po $*.c.i

POFILES= $(OBJS:%.o=%.po)
POFILE= $(PROG)_cmd.po

.KEEP_STATE:

all: $(PROG)

$(PROG): $(OBJS)
	$(LINK.c) $(OBJS) -o $@ $(LDLIBS)
	$(POST_PROCESS)

$(POFILE): $(POFILES)
	$(RM) $@
	cat $(POFILES) > $@

install: all $(ROOTPROG)

clean:
	$(RM) $(OBJS)

lint:	lint_SRCS

include ../Makefile.targ
