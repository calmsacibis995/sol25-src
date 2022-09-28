/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)mvwin.c	1.5	92/07/14 SMI"	/* SVr4.0 1.9	*/

#include	"curses_inc.h"

/* relocate the starting position of a _window */

mvwin(win, by, bx)
register	WINDOW	*win;
register	int	by, bx;
{
    if ((by + win->_maxy) > LINES || (bx + win->_maxx) > COLS ||
	 by < 0 || bx < 0)
         return ERR;
    win->_begy = by;
    win->_begx = bx;
    (void) wtouchln(win, 0, win->_maxy, -1);
    return (win->_immed ? wrefresh(win) : OK);
}
