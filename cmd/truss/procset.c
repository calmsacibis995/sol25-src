 /*
  * Copyright (c) 1992 Sun Microsystems, Inc.  All Rights Reserved.
  */

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#pragma ident "@(#)procset.c	1.9	94/01/17 SMI"	/* SVr4.0 1.2	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/signal.h>
#include <sys/fault.h>
#include <sys/syscall.h>
#include "pcontrol.h"
#include "ramdata.h"
#include "proto.h"

#include <sys/wait.h>

/*
 * Function prototypes for static routines in this module.
 */
#if	defined(__STDC__)

static	CONST char *	idop_enum( idop_t );
CONST char *	idtype_enum( idtype_t );

#else	/* defined(__STDC__) */

static	CONST char *	idop_enum();
CONST char *	idtype_enum();

#endif	/* defined(__STDC__) */

void
show_procset(Pr, offset)
	process_t *Pr;
	long offset;
{
	procset_t procset;
	register procset_t * psp = &procset;

	if (Pread(Pr, offset, (char *)psp, sizeof(*psp)) == sizeof(*psp)) {
		(void) printf("%s\top=%s",
			pname, idop_enum(psp->p_op));
		(void) printf("  ltyp=%s lid=%ld",
			idtype_enum(psp->p_lidtype), (long)psp->p_lid);
		(void) printf("  rtyp=%s rid=%ld\n",
			idtype_enum(psp->p_ridtype), (long)psp->p_rid);
	}
}

CONST char *
idop_enum(arg)
	idop_t arg;
{
	register CONST char * str;

	switch (arg) {
	case POP_DIFF:	str = "POP_DIFF";	break;
	case POP_AND:	str = "POP_AND";	break;
	case POP_OR:	str = "POP_OR";		break;
	case POP_XOR:	str = "POP_XOR";	break;
	default:	(void) sprintf(code_buf, "%d", arg);
			str = (CONST char *)code_buf;
			break;
	}

	return str;
}

CONST char *
idtype_enum(arg)
	idtype_t arg;
{
	register CONST char * str;

	switch (arg) {
	case P_PID:	str = "P_PID";		break;
	case P_PPID:	str = "P_PPID";		break;
	case P_PGID:	str = "P_PGID";		break;
	case P_SID:	str = "P_SID";		break;
	case P_CID:	str = "P_CID";		break;
	case P_UID:	str = "P_UID";		break;
	case P_GID:	str = "P_GID";		break;
	case P_ALL:	str = "P_ALL";		break;
	case P_LWPID:	str = "P_LWPID";	break;
	default:	(void) sprintf(code_buf, "%d", arg);
			str = (CONST char *)code_buf;
			break;
	}

	return str;
}

CONST char *
woptions(arg)
register int arg;
{
	register char * str = code_buf;

	if (arg == 0)
		return "0";
	if (arg & ~(WEXITED|WTRAPPED|WSTOPPED|WCONTINUED|WNOHANG|WNOWAIT)) {
		(void) sprintf(str, "0%-6o", arg);
		return (CONST char *)str;
	}

	*str = '\0';
	if (arg & WEXITED)
		(void) strcat(str, "|WEXITED");
	if (arg & WTRAPPED)
		(void) strcat(str, "|WTRAPPED");
	if (arg & WSTOPPED)
		(void) strcat(str, "|WSTOPPED");
	if (arg & WCONTINUED)
		(void) strcat(str, "|WCONTINUED");
	if (arg & WNOHANG)
		(void) strcat(str, "|WNOHANG");
	if (arg & WNOWAIT)
		(void) strcat(str, "|WNOWAIT");

	return (CONST char *)(str+1);
}

void
show_statloc(Pr, offset)
	process_t *Pr;
	long offset;
{
	int status;

	if (Pread(Pr, offset, (char *)&status, sizeof(status)) ==
	    sizeof(status))
		(void) printf("%s\tstatus=0x%.4X\n", pname, status);
}
