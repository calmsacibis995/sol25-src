#
#ident	"@(#)Makefile	1.6	92/12/15 SMI"
#
# Copyright (c) 1989 by Sun Microsystems, Inc.
#

PROG= units
UNITTAB=unittab

include ../Makefile.cmd
LDLIBS += -lintl

LIBUNITTAB= $(UNITTAB:%=$(ROOTSHLIB)/%)
$(LIBUNITTAB) :=	FILEMODE =	$(LIBFILEMODE)

.KEEP_STATE:

all: $(PROG) unittab

install: all $(ROOTPROG) $(LIBUNITTAB)

clean:

lint:	lint_PROG

include ../Makefile.targ
