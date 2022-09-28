/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)getwidth.c	1.5	92/07/14 SMI"	/* SVr4.0 1.4	*/

#include "libw.h"
#include "_wchar.h"
#include <ctype.h>

void getwidth(eucstruct)
eucwidth_t *eucstruct;
{
	eucstruct->_eucw1 = eucw1;
	eucstruct->_eucw2 = eucw2;
	eucstruct->_eucw3 = eucw3;
	eucstruct->_multibyte = multibyte;
	if (_ctype[520] > 3 || eucw1 > 2)
		eucstruct->_pcw = sizeof(unsigned long);
	else
		eucstruct->_pcw = sizeof(unsigned short);
	eucstruct->_scrw1 = scrw1;
	eucstruct->_scrw2 = scrw2;
	eucstruct->_scrw3 = scrw3;
}
