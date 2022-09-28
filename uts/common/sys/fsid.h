/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifndef _SYS_FSID_H
#define	_SYS_FSID_H

#pragma ident	"@(#)fsid.h	1.10	92/07/14 SMI"	/* SVr4.0 11.2	*/

#ifdef	__cplusplus
extern "C" {
#endif

/* Fstyp names for use in fsinfo structure. These names */
/* must be constant across releases and will be used by a */
/* user level routine to map fstyp to fstyp index as used */
/* ip->i_fstyp. This is necessary for programs like mount. */

#define	S51K	"S51K"
#define	PROC	"PROC"
#define	DUFST	"DUFST"
#define	NFS	"NFS"
#define	S52K	"S52K"

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_FSID_H */
