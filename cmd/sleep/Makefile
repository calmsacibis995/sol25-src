#
#ident	"@(#)Makefile	1.6	94/11/16 SMI"
#
# Copyright (c) 1989 by Sun Microsystems, Inc.
#

PROG= sleep

include ../Makefile.cmd
CFLAGS += -v
LDLIBS += -lintl

.KEEP_STATE:

all: $(PROG)

install: all $(ROOTPROG)

clean:

lint:	lint_PROG

include ../Makefile.targ
