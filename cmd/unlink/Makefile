#
#ident	"@(#)Makefile	1.7	94/08/25 SMI"
#
# Copyright (c) 1989 by Sun Microsystems, Inc.
#

PROG= unlink

include ../Makefile.cmd
LDLIBS += -lintl

FILEMODE = 0555
OWNER = root

.KEEP_STATE:

all: $(PROG) 

install: all $(ROOTUSRSBINPROG)
	-$(RM) $(ROOTETCPROG)
	-$(SYMLINK) ../usr/sbin/$(PROG) $(ROOTETCPROG)

clean:

lint:	lint_PROG

include ../Makefile.targ
