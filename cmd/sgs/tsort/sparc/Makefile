#
#ident	"@(#)Makefile	1.5	94/11/08 SMI"
#
# Copyright (c) 1993 by Sun Microsystems, Inc.
#
# cmd/sgs/tsort/sparc/Makefile
#

PROG=		tsort

include 	../../../Makefile.cmd

OBJS=		Err.o erraction.o errafter.o errbefore.o errexit.o \
		errprefix.o errsource.o errstrtok.o errtag.o errtext.o \
		errtofix.o errusage.o tsort.o zfopen.o zmalloc.o

SRCS=		$(OBJS:%.o=../common/%.c)

DEFLIST=	-DELF
CPPFLAGS=	$(INCLIST) $(DEFLIST) $(CPPFLAGS.master)

CLEANFILES +=	$(LINTOUT)
LDLIBS += -lintl

%.o:		../common/%.c
		$(COMPILE.c) $<

.KEEP_STATE:

all:		$(PROG)

$(PROG):	$(OBJS)
		$(LINK.c) $(OBJS) -o $@ $(LDLIBS)
		$(POST_PROCESS)

install:	all $(ROOTCCSBINPROG)

clean:
		$(RM) $(OBJS) $(CLEANFILES)

lint:		$(LINTOUT)

$(LINTOUT):	$(SRCS)
		$(LINT.c) $(SRCS) > $(LINTOUT) 2>&1

include		../../../Makefile.targ
