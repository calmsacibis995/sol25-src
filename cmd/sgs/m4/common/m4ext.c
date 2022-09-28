/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#pragma ident	"@(#)m4ext.c	6.5	95/03/21 SMI"

#include	"stdio.h"
#include	"m4.h"


/* storage params */
int	hshsize 	= 199;		/* hash table size (prime) */
int	bufsize 	= 4096;		/* pushback & arg text buffers */
int	stksize 	= 100;		/* call stack */
int	toksize 	= 512;		/* biggest word ([a-z_][a-z0-9_]*) */


/* pushback buffer */
char	*ibuf;				/* buffer */
char	*ibuflm;			/* highest buffer addr */
char	*ip;				/* current position */
char	*ipflr;				/* buffer floor */
char 	*ipstk[10];			/* stack for "ipflr"s */


/* arg collection buffer */
char	*obuf;				/* buffer */
char	*obuflm;			/* high address */
char	*op;				/* current position */


/* call stack */
struct call	*callst;		/* stack */
struct call	*Cp 	= NULL;		/* position */


/* token storage */
char	*token;				/* buffer */
char	*toklm;				/* high addr */


/* file name and current line storage for line sync and diagnostics */
char	fnbuf[200];			/* holds file name strings */
char	*fname[11] 	= {fnbuf};	/* file name ptr stack */
int	fline[10];			/* current line nbr stack */


/* input file stuff for "include"s */
FILE	*ifile[10] 	= {stdin};	/* stack */
int	ifx;				/* stack index */


/* stuff for output diversions */
FILE	*cf 	= stdout;		/* current output file */
FILE	*ofile[11] 	= {stdout};	/* output file stack */
int	ofx;				/* stack index */


/* comment markers */
char	lcom[MAXSYM+1] 	= "#";
char	rcom[MAXSYM+1] 	= "\n";


/* quote markers */
char	lquote[MAXSYM+1] 	= "`";
char	rquote[MAXSYM+1] 	= "'";


/* argument ptr stack */
char	**argstk;
char	*astklm;			/* high address */
char	**Ap;				/* current position */


/* symbol table */
struct nlist	**hshtab;		/* hash table */
int	hshval;				/* last hash val */


/* misc */
char	*procnam;			/* argv[0] */
char	*tempfile;			/* used for diversion files */
struct	Wrap *wrapstart = NULL;	/* first entry in of list of "m4wrap" strings */
int	C;				/* see "m4.h" macros */
char	nullstr[] 	= "";
int	nflag 	= 1;			/* name flag, used for line sync code */
int	sflag;				/* line sync flag */
int	sysrval;			/* return val from syscmd */
int	trace;				/* global trace flag */
int	exitstat = OK;			/* global exit status */
int	flushio = 0;			/* flag - flush std I/O buffers */
