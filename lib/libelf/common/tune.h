/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#pragma ident	"@(#)tune.h	1.4	92/08/20 SMI" 	/* SVr4.0 1.1	*/


/* Tunable parameters
 *	This file defines parameters that one can change to improve
 *	performance for particular machines.
 *
 *	PGSZ	Used in input.c as the size of "pages" to read from
 *		the file.  Generally, smaller sizes give the opportunity
 *		to read less; larger sizes make the reads that happen
 *		more efficient.
 *
 *		Recommendation:  Use at least the block size for the most
 *		common file systems on the machine.  Twice the common
 *		size seems to work well, 4 blocks seems a little too big.
 *		SVR3 uses 1k blocks, SVR4 uses 2K.
 */


#define PGSZ	4096
