#
#ident	"@(#)Makefile	1.8	94/12/05 SMI"
#
# Copyright (c) 1989 by Sun Microsystems, Inc.
#

PROG= od
XPG4PROG= od

include ../Makefile.cmd

.KEEP_STATE:

LDLIBS += -lw -lintl
CFLAGS += -v
$(XPG4):= CPPFLAGS += -DXPG4
XGETFLAGS += -a -x od.xcl

all: $(PROG) $(XPG4)

install: all $(ROOTPROG) $(ROOTXPG4PROG)

clean:

lint:	lint_PROG

include ../Makefile.targ
