#
# uts/sun4e/vmemem/Makefile
# Copyright (c) 1991 by Sun Microsystems, Inc.
#
#ident	"@(#)Makefile 1.7     93/07/14 SMI"
#
#	This makefile drives the production of the vmemem driver kernel module.
#
#	sun4e implementation architecture dependent
#

#
#	Path to the base of the uts directory tree (usually /usr/src/uts).
#
UTSBASE	= ../..

#
#	Define the module and object file sets.
#
MODULE		= vmemem
OBJECTS		= $(VMEMEM_OBJS:%=$(OBJS_DIR)/%)
LINTS		= $(VMEMEM_OBJS:%.o=$(LINTS_DIR)/%.ln)
ROOTMODULE	= $(ROOT_PSM_DRV_DIR)/$(MODULE)
CONF_SRCDIR	= $(UTSBASE)/sun/io

#
#	Include common rules.
#
include $(UTSBASE)/sun4e/Makefile.sun4e

CLOBBERFILES	+= $(MODULE)

#
# lint pass one enforcement
#
CFLAGS += -v

#
#	Default build targets.
#
.KEEP_STATE:

all:		$(MODULE) $(SRC_CONFILE)

lint:		$(MODULE).lint

install:	$(MODULE) $(ROOTMODULE) $(ROOT_CONFFILE)

#
#	Include common targets.
#
include $(UTSBASE)/sun4e/Makefile.targ
