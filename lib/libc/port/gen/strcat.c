/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)strcat.c	1.7	92/07/14 SMI"	/* SVr4.0 1.6	*/

/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
#include "synonyms.h"
#include <string.h>

/*
 * Concatenate s2 on the end of s1.  S1's space must be large enough.
 * Return s1.
 */

char *
strcat(s1, s2)
register char *s1;
#ifdef __STDC__
register const char *s2;
#else
register char *s2;
#endif
{
	char *os1 = s1;

	while(*s1++)
		;
	--s1;
	while(*s1++ = *s2++)
		;
	return(os1);
}
