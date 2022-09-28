/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)field_user.c	1.2	92/07/14 SMI"	/* SVr4.0 1.1	*/

#include "utility.h"

	/**********************
	*  set_field_userptr  *
	**********************/

int set_field_userptr (f, userptr)
FIELD * f;
char * userptr;
{
	Field (f) -> usrptr = userptr;
	return E_OK;
}

char * field_userptr (f)
FIELD * f;
{
	return Field (f) -> usrptr;
}

