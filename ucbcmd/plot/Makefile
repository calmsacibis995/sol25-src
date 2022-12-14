#
#ident	"@(#)Makefile	1.5	92/01/13 SMI"
#
# Copyright (c) 1989 by Sun Microsystems, Inc.
#

PROG=	tek t4013 t300 t300s t450 aedplot bgplot dumbplot gigiplot \
	hpplot hp7221plot implot atoplot plottoa vplot crtplot plot

SUBDIRS= libplot

DRVOBJ=		driver.o
VPLOTOBJ=	vplot.o chrtab.o
CRTOBJ= 	crtplot.o crtdriver.o
ATOOBJ=		atoplot.o
TOAOBJ=		plottoa.o

OBJS=  $(DRVOBJ) $(VPLOTOBJ) $(CRTOBJ) $(ATOOBJ) $(TOAOBJ)

DRVSRC= $(DRVOBJ:%.o=%.c)
VPLOTSRC= $(VPLOTOBJ:%.o=%.c)
CRTSRC=  $(CRTOBJ:%.o=%.c)
ATOSRC=  $(ATOOBJ:%.o=%.c)
TOASRC=  $(TOAOBJ:%.o=%.c)

SRCS=  $(DRVSRC) $(VPLOTSRC) $(CRTSRC) $(ATOSRC) $(TOASRC)

include ../Makefile.ucbcmd

FILEMODE=	755

INC1 = $(ROOT)/usr/ucbinclude

LDFLAGS +=  -L$(ROOT)/usr/ucblib 

LIBPLOT = ./libplot

LIBS.DEF = -lm -lucb -lelf
LIBS.PLOT = 
LDLIBS += $(LIBS.PLOT) $(LIBS.DEF)

#CPPFLAGS.master += -I$(INC1)
CPPFLAGS = -I$(INC1) $(CPPFLAGS.master)

clean:=		TARGET= clean
clobber:=	TARGET= clobber

.KEEP_STATE:

all: $(PROG) debug

tek:= LDFLAGS += -L$(LIBPLOT)
tek:= LIBS.PLOT = -lt4014
tek:	$(DRVOBJ) $(LIBPLOT)/libt4014.a
	$(SETRUNPATH) $(LINK.c) $(DRVOBJ) -o $@ $(LDLIBS)
	$(POST_PROCESS)

t4013:= LDFLAGS += -L$(LIBPLOT)
t4013:= LIBS.PLOT = -lt4013
t4013:	$(DRVOBJ) $(LIBPLOT)/libt4013.a
	$(SETRUNPATH) $(LINK.c) $(DRVOBJ) -o $@ $(LDLIBS)
	$(POST_PROCESS)

t300:= LDFLAGS += -L$(LIBPLOT)
t300:= LIBS.PLOT = -lt300
t300:	$(DRVOBJ) $(LIBPLOT)/libt300.a
	$(SETRUNPATH) $(LINK.c) $(DRVOBJ) -o $@ $(LDLIBS)
	$(POST_PROCESS)

t300s:= LDFLAGS += -L$(LIBPLOT)
t300s:= LIBS.PLOT = -lt300s
t300s:	$(DRVOBJ) $(LIBPLOT)/libt300s.a
	$(SETRUNPATH) $(LINK.c) $(DRVOBJ) -o $@ $(LDLIBS)
	$(POST_PROCESS)

t450:= LDFLAGS += -L$(LIBPLOT)
t450:= LIBS.PLOT = -lt450
t450:	$(DRVOBJ) $(LIBPLOT)/libt450.a
	$(SETRUNPATH) $(LINK.c) $(DRVOBJ) -o $@ $(LDLIBS)
	$(POST_PROCESS)

vplot:	$(VPLOTOBJ)
	$(SETRUNPATH) $(LINK.c) $(VPLOTOBJ) -o $@ $(LDLIBS)
	$(POST_PROCESS)

crtplot:= LDFLAGS += -L$(LIBPLOT)
crtplot:= LIBS.DEF = -lcurses -ltermcap -lm -lucb -lelf
crtplot:  $(CRTOBJ)
	$(SETRUNPATH) $(LINK.c) $(CRTOBJ) -o $@ $(LDLIBS)
	$(POST_PROCESS)

aedplot:= LDFLAGS += -L$(LIBPLOT)
aedplot:= LIBS.PLOT = -laed
aedplot:	$(DRVOBJ) $(LIBPLOT)/libaed.a
	$(SETRUNPATH) $(LINK.c) $(DRVOBJ) -o $@ $(LDLIBS)
	$(POST_PROCESS)

bgplot:= LDFLAGS += -L$(LIBPLOT)
bgplot:= LIBS.PLOT = -lplotbg
bgplot:	$(DRVOBJ) $(LIBPLOT)/libplotbg.a
	$(SETRUNPATH) $(LINK.c) $(DRVOBJ) -o $@ $(LDLIBS)
	$(POST_PROCESS)

dumbplot:= LDFLAGS += -L$(LIBPLOT)
dumbplot:= LIBS.DEF = -ltermcap -lm -lucb -lelf
dumbplot:= LIBS.PLOT = -ldumb
dumbplot:	$(DRVOBJ) $(LIBPLOT)/libdumb.a
	$(SETRUNPATH) $(LINK.c) $(DRVOBJ) -o $@ $(LDLIBS)
	$(POST_PROCESS)

gigiplot:= LDFLAGS += -L$(LIBPLOT)
gigiplot:= LIBS.PLOT = -lgigi
gigiplot:	$(DRVOBJ) $(LIBPLOT)/libgigi.a
	$(SETRUNPATH) $(LINK.c) $(DRVOBJ) -o $@ $(LDLIBS)
	$(POST_PROCESS)

hpplot:= LDFLAGS += -L$(LIBPLOT)
hpplot:= LIBS.PLOT = -lhp2648
hpplot:	$(DRVOBJ) $(LIBPLOT)/libhp2648.a
	$(SETRUNPATH) $(LINK.c) $(DRVOBJ) -o $@ $(LDLIBS)
	$(POST_PROCESS)

hp7221plot:= LDFLAGS += -L$(LIBPLOT)
hp7221plot:= LIBS.PLOT = -lhp7221
hp7221plot:	$(DRVOBJ) $(LIBPLOT)/libhp7221.a
	$(SETRUNPATH) $(LINK.c) $(DRVOBJ) -o $@ $(LDLIBS)
	$(POST_PROCESS)

implot:= LDFLAGS += -L$(LIBPLOT)
implot:= LIBS.PLOT = -limagen
implot:	$(DRVOBJ) $(LIBPLOT)/libimagen.a
	$(SETRUNPATH) $(LINK.c) $(DRVOBJ) -o $@ $(LDLIBS)
	$(POST_PROCESS)

atoplot:= LDFLAGS += -L$(LIBPLOT)
atoplot:= LIBS.PLOT = -lplot
atoplot:  $(ATOOBJ) $(LIBPLOT)/libplot.a
	$(SETRUNPATH) $(LINK.c) $(ATOOBJ) -o $@ $(LDLIBS)
	$(POST_PROCESS)

plottoa:  $(TOAOBJ)
	$(SETRUNPATH) $(LINK.c) $(TOAOBJ) -o $@ $(LDLIBS)
	$(POST_PROCESS)

$(LIBPLOT)/libt300.a:	FRC
	cd $(LIBPLOT); pwd; $(MAKE) t300

$(LIBPLOT)/libt300s.a: 	FRC
	cd $(LIBPLOT); pwd; $(MAKE) t300s

$(LIBPLOT)/libt450.a: 	FRC
	cd $(LIBPLOT); pwd; $(MAKE) t450

$(LIBPLOT)/libt4014.a: 	FRC
	cd $(LIBPLOT); pwd; $(MAKE) t4014

$(LIBPLOT)/libaed.a:	FRC
	cd $(LIBPLOT); pwd; $(MAKE) aed

$(LIBPLOT)/libplotbg.a:	FRC
	cd $(LIBPLOT); pwd; $(MAKE) bitgraph

$(LIBPLOT)/libdumb.a:	FRC
	cd $(LIBPLOT); pwd; $(MAKE) dumb

$(LIBPLOT)/libgigi.a:	FRC
	cd $(LIBPLOT); pwd; $(MAKE) gigi

$(LIBPLOT)/libhp2648.a:	FRC
	cd $(LIBPLOT); pwd; $(MAKE) hp2648

$(LIBPLOT)/libhp7221.a:	FRC
	cd $(LIBPLOT); pwd; $(MAKE) hp7221

$(LIBPLOT)/libimagen.a:	FRC
	cd $(LIBPLOT); pwd; $(MAKE) imagen

$(LIBPLOT)/libplot.a:	FRC
	cd $(LIBPLOT); pwd; $(MAKE) plot

$(LIBPLOT)/libt4013.a:	FRC
	cd $(LIBPLOT); pwd; $(MAKE) t4013

install: all $(ROOTPROG)

clean:  $(SUBDIRS) clean_all

clobber:  $(SUBDIRS) clobber_all

clean_all:
	$(RM) $(OBJS)

clobber_all:
	$(RM) $(PROG)

$(SUBDIRS):  FRC
	@cd $@; pwd; $(MAKE) $(MFLAGS) $(TARGET)

lint:	lint_PROG

include ../Makefile.ucbtarg

FRC:

