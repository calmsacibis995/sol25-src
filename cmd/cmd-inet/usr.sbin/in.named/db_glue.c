/*
 * Copyright (c) 1986, 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted provided
 * that: (1) source distributions retain this entire copyright notice and
 * comment, and (2) distributions including binaries display the following
 * acknowledgement:  ``This product includes software developed by the
 * University of California, Berkeley and its contributors'' in the
 * documentation or other materials provided with the distribution and in
 * all advertising materials mentioning features or use of this software.
 * Neither the name of the University nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#pragma ident	"@(#)db_glue.c	1.3	93/04/16 SMI"

#include <sys/param.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <stdio.h>
#include <syslog.h>
#include <ctype.h>
#include <netdb.h>
#include <arpa/nameser.h>
#include "ns.h"
#include "db.h"

struct valuelist {
	struct valuelist *next, *prev;
	char	*name;
	char	*proto;
	short	port;
} *servicelist, *protolist;

void
buildservicelist()
{
	struct servent *sp;
	struct valuelist *slp;

	setservent(1);
	while (sp = getservent()) {
		slp = (struct valuelist *)malloc(sizeof (struct valuelist));
		slp->name = savestr(sp->s_name);
		slp->proto = savestr(sp->s_proto);
		slp->port = ntohs((u_short)sp->s_port);
		slp->next = servicelist;
		slp->prev = NULL;
		if (servicelist)
			servicelist->prev = slp;
		servicelist = slp;
	}
	endservent();
}

void
buildprotolist()
{
	struct protoent *pp;
	struct valuelist *slp;

	setprotoent(1);
	while (pp = getprotoent()) {
		slp = (struct valuelist *)malloc(sizeof (struct valuelist));
		slp->name = savestr(pp->p_name);
		slp->port = pp->p_proto;
		slp->next = protolist;
		slp->prev = NULL;
		if (protolist)
			protolist->prev = slp;
		protolist = slp;
	}
	endprotoent();
}

/*
 * Convert service name or (ascii) number to int.
 */
servicenumber(p)
	char *p;
{

	return (findservice(p, &servicelist));
}

/*
 * Convert protocol name or (ascii) number to int.
 */
protocolnumber(p)
	char *p;
{

	return (findservice(p, &protolist));
}

findservice(s, list)
	register char *s;
	register struct valuelist **list;
{
	register struct valuelist *lp = *list;
	int n;

	for (; lp != NULL; lp = lp->next)
		if (strcasecmp(lp->name, s) == 0) {
			if (lp != *list) {
				lp->prev->next = lp->next;
				if (lp->next)
					lp->next->prev = lp->prev;
				(*list)->prev = lp;
				lp->next = *list;
				*list = lp;
			}
			return (lp->port);
		}
	if (sscanf(s, "%d", &n) != 1 || n <= 0)
		n = -1;
	return (n);
}

struct servent *
cgetservbyport(port, proto)
	u_short port;
	char *proto;
{
	register struct valuelist **list = &servicelist;
	register struct valuelist *lp = *list;
	static struct servent serv;

	port = htons(port);
	for (; lp != NULL; lp = lp->next) {
		if (port != lp->port)
			continue;
		if (strcasecmp(lp->proto, proto) == 0) {
			if (lp != *list) {
				lp->prev->next = lp->next;
				if (lp->next)
					lp->next->prev = lp->prev;
				(*list)->prev = lp;
				lp->next = *list;
				*list = lp;
			}
			serv.s_name = lp->name;
			serv.s_port = htons((u_short)lp->port);
			serv.s_proto = lp->proto;
			return (&serv);
		}
	}
	return (0);
}

struct protoent *
cgetprotobynumber(proto)
	register int proto;
{

	register struct valuelist **list = &protolist;
	register struct valuelist *lp = *list;
	static struct protoent prot;

	for (; lp != NULL; lp = lp->next)
		if (lp->port == proto) {
			if (lp != *list) {
				lp->prev->next = lp->next;
				if (lp->next)
					lp->next->prev = lp->prev;
				(*list)->prev = lp;
				lp->next = *list;
				*list = lp;
			}
			prot.p_name = lp->name;
			prot.p_proto = lp->port;
			return (&prot);
		}
	return (0);
}

char *
protocolname(num)
	int num;
{
	static	char number[8];
	struct protoent *pp;

	pp = cgetprotobynumber(num);
	if (pp == 0)  {
		(void) sprintf(number, "%d", num);
		return (number);
	}
	return (pp->p_name);
}

char *
servicename(port, proto)
	u_short port;
	char *proto;
{
	static	char number[8];
	struct servent *ss;

	ss = cgetservbyport(htons(port), proto);
	if (ss == 0)  {
		(void) sprintf(number, "%d", port);
		return (number);
	}
	return (ss->s_name);
}
