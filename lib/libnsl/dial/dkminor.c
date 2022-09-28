/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)dkminor.c	1.5	92/07/14 SMI"	/* SVr4.0 1.1	*/

#ifndef DIAL
	static char	SCCSID[] = "@(#)dkminor.c	2.3+BNU DKHOST 86/12/02";
#endif
/*
 *	COMMKIT(TM) Software - Datakit(R) VCS Interface Release 2.0 V1
 *			Copyright 1984 AT&T
 *			All Rights Reserved
 *
 *	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
 *     The copyright notice above does not evidence any actual
 *          or intended publication of such source code.
 */
/*
   Return minor device number for a given Datakit device.
   The channel number is used since using the minor device returned
   by fstat gives wrong results for duplex systems.
*/

#include "dk.h"
#include <rpc/trace.h>


GLOBAL
dkminor(fd)
{
	struct diocreq iocb;


	trace1(TR_dkminor, 0);
	if (ioctl(fd, DIOCINFO, &iocb) < 0) {
		trace1(TR_dkminor, 1);
		return (-1);
	}
	trace1(TR_dkminor, 1);
	return (iocb.req_chmin); /* req_chmin contains channel number */
}
