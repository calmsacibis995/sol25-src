/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*
 * Copyright  (c) 1986 AT&T
 *	All Rights Reserved
 */
#ident	"@(#)memshift.c	1.4	92/07/14 SMI"       /* SVr4.0 1.1 */

#include	<stdio.h>
#include	"wish.h"

char *
memshift(dst, src, len)
char	*dst;
char	*src;
int	len;
{
	register char	*d;
	register char	*s;
	register int	n;
	extern char	*memcpy();

	if (dst < src)
		if (dst + len <= src)
			memcpy(dst, src, len);
		else
			for (s = src, d = dst, n = len; n > 0; n--)
				*d++ = *s++;
	else
		if (src + len <= dst)
			return memcpy(dst, src, len);
		else
			for (n = len, s = src + n, d = dst + n; n > 0; n--)
				*--d = *--s;
	return dst;
}
