#
#ident	"@(#)Makefile	1.5	92/12/15 SMI"
#
# Copyright (c) 1989 by Sun Microsystems, Inc.
#

PROG= dsconfig

SRCS= $(PROG:%=%.sh)

include ../Makefile.cmd

.KEEP_STATE:

all: $(PROG) 

install: all $(ROOTPROG)

clean:

lint:

include ../Makefile.targ
