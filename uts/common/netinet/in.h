/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that this notice is preserved and that due credit is given
 * to the University of California at Berkeley. The name of the University
 * may not be used to endorse or promote products derived from this
 * software without specific prior written permission. This software
 * is provided ``as is'' without express or implied warranty.
 */

/*
 * Constants and structures defined by the internet system,
 * Per RFC 790, September 1981.
 */

#ifndef _NETINET_IN_H
#define	_NETINET_IN_H

#pragma ident	"@(#)in.h	1.4	93/07/06 SMI"
/* in.h 1.19 90/07/27 SMI; from UCB 7.5 2/22/88	*/

#ifdef	__cplusplus
extern "C" {
#endif

#include <sys/stream.h>
#include <sys/byteorder.h>

/*
 * Protocols
 */
#define	IPPROTO_IP		0		/* dummy for IP */
#define	IPPROTO_ICMP		1		/* control message protocol */
#define	IPPROTO_IGMP		2		/* group control protocol */
#define	IPPROTO_GGP		3		/* gateway^2 (deprecated) */
#define	IPPROTO_ENCAP		4		/* IP in IP encapsulation */
#define	IPPROTO_TCP		6		/* tcp */
#define	IPPROTO_EGP		8		/* exterior gateway protocol */
#define	IPPROTO_PUP		12		/* pup */
#define	IPPROTO_UDP		17		/* user datagram protocol */
#define	IPPROTO_IDP		22		/* xns idp */
#define	IPPROTO_HELLO		63		/* "hello" routing protocol */
#define	IPPROTO_ND		77		/* UNOFFICIAL net disk proto */
#define	IPPROTO_EON		80		/* ISO clnp */

#define	IPPROTO_RAW		255		/* raw IP packet */
#define	IPPROTO_MAX		256

/*
 * Port/socket numbers: network standard functions
 */
#define	IPPORT_ECHO		7
#define	IPPORT_DISCARD		9
#define	IPPORT_SYSTAT		11
#define	IPPORT_DAYTIME		13
#define	IPPORT_NETSTAT		15
#define	IPPORT_FTP		21
#define	IPPORT_TELNET		23
#define	IPPORT_SMTP		25
#define	IPPORT_TIMESERVER	37
#define	IPPORT_NAMESERVER	42
#define	IPPORT_WHOIS		43
#define	IPPORT_MTP		57

/*
 * Port/socket numbers: host specific functions
 */
#define	IPPORT_TFTP		69
#define	IPPORT_RJE		77
#define	IPPORT_FINGER		79
#define	IPPORT_TTYLINK		87
#define	IPPORT_SUPDUP		95

/*
 * UNIX TCP sockets
 */
#define	IPPORT_EXECSERVER	512
#define	IPPORT_LOGINSERVER	513
#define	IPPORT_CMDSERVER	514
#define	IPPORT_EFSSERVER	520

/*
 * UNIX UDP sockets
 */
#define	IPPORT_BIFFUDP		512
#define	IPPORT_WHOSERVER	513
#define	IPPORT_ROUTESERVER	520	/* 520+1 also used */

/*
 * Ports < IPPORT_RESERVED are reserved for
 * privileged processes (e.g. root).
 * Ports > IPPORT_USERRESERVED are reserved
 * for servers, not necessarily privileged.
 */
#define	IPPORT_RESERVED		1024
#define	IPPORT_USERRESERVED	5000

/*
 * Link numbers
 */
#define	IMPLINK_IP		155
#define	IMPLINK_LOWEXPER	156
#define	IMPLINK_HIGHEXPER	158

/*
 * Internet address
 *	This definition contains obsolete fields for compatibility
 *	with SunOS 3.x and 4.2bsd.  The presence of subnets renders
 *	divisions into fixed fields misleading at best.	 New code
 *	should use only the s_addr field.
 */
struct in_addr {
	union {
		struct { u_char s_b1, s_b2, s_b3, s_b4; } S_un_b;
		struct { u_short s_w1, s_w2; } S_un_w;
		u_long S_addr;
	} S_un;
#define	s_addr	S_un.S_addr		/* should be used for all code */
#define	s_host	S_un.S_un_b.s_b2	/* OBSOLETE: host on imp */
#define	s_net	S_un.S_un_b.s_b1	/* OBSOLETE: network */
#define	s_imp	S_un.S_un_w.s_w2	/* OBSOLETE: imp */
#define	s_impno	S_un.S_un_b.s_b4	/* OBSOLETE: imp # */
#define	s_lh	S_un.S_un_b.s_b3	/* OBSOLETE: logical host */
};

/*
 * Definitions of bits in internet address integers.
 * On subnets, the decomposition of addresses to host and net parts
 * is done according to subnet mask, not the masks here.
 */
#define	IN_CLASSA(i)		(((long)(i) & 0x80000000) == 0)
#define	IN_CLASSA_NET		0xff000000
#define	IN_CLASSA_NSHIFT	24
#define	IN_CLASSA_HOST		0x00ffffff
#define	IN_CLASSA_MAX		128

#define	IN_CLASSB(i)		(((long)(i) & 0xc0000000) == 0x80000000)
#define	IN_CLASSB_NET		0xffff0000
#define	IN_CLASSB_NSHIFT	16
#define	IN_CLASSB_HOST		0x0000ffff
#define	IN_CLASSB_MAX		65536

#define	IN_CLASSC(i)		(((long)(i) & 0xe0000000) == 0xc0000000)
#define	IN_CLASSC_NET		0xffffff00
#define	IN_CLASSC_NSHIFT	8
#define	IN_CLASSC_HOST		0x000000ff

#define	IN_CLASSD(i)		(((long)(i) & 0xf0000000) == 0xe0000000)
#define	IN_CLASSD_NET		0xf0000000	/* These ones aren't really */
#define	IN_CLASSD_NSHIFT	28		/* net and host fields, but */
#define	IN_CLASSD_HOST		0x0fffffff	/* routing needn't know.    */
#define	IN_MULTICAST(i)		IN_CLASSD(i)

#define	IN_EXPERIMENTAL(i)	(((long)(i) & 0xe0000000) == 0xe0000000)
#define	IN_BADCLASS(i)		(((long)(i) & 0xf0000000) == 0xf0000000)

#define	INADDR_ANY		(u_long)0x00000000
#define	INADDR_LOOPBACK		(u_long)0x7F000001
#define	INADDR_BROADCAST	(u_long)0xffffffff	/* must be masked */

#define	INADDR_UNSPEC_GROUP	(u_long)0xe0000000	/* 224.0.0.0   */
#define	INADDR_ALLHOSTS_GROUP	(u_long)0xe0000001	/* 224.0.0.1   */
#define	INADDR_MAX_LOCAL_GROUP	(u_long)0xe00000ff	/* 224.0.0.255 */

#define	IN_LOOPBACKNET		127			/* official! */

/*
 * Define a macro to stuff the loopback address into an Internet address
 */
#define	IN_SET_LOOPBACK_ADDR(a) \
	{ (a)->sin_addr.s_addr  = htonl(INADDR_LOOPBACK); \
	(a)->sin_family = AF_INET; }

/*
 * Socket address, internet style.
 */
struct sockaddr_in {
	short	sin_family;
	u_short	sin_port;
	struct	in_addr sin_addr;
	char	sin_zero[8];
};

/*
 * Options for use with [gs]etsockopt at the IP level.
 */
#define	IP_OPTIONS	1	/* set/get IP per-packet options   */
#define	IP_HDRINCL	2	/* int; header is included with data (raw) */
#define	IP_TOS		3	/* int; IP type of service and precedence */
#define	IP_TTL		4	/* int; IP time to live */
#define	IP_RECVOPTS	5	/* bool; receive all IP options w/datagram */
#define	IP_RECVRETOPTS	6	/* bool; receive IP options for response */
#define	IP_RECVDSTADDR	7	/* bool; receive IP dst addr w/datagram */
#define	IP_RETOPTS	8	/* ip_opts; set/get IP per-packet options */
#define	IP_MULTICAST_IF		0x10	/* set/get IP multicast interface  */
#define	IP_MULTICAST_TTL	0x11	/* set/get IP multicast timetolive */
#define	IP_MULTICAST_LOOP	0x12	/* set/get IP multicast loopback   */
#define	IP_ADD_MEMBERSHIP	0x13	/* add	an IP group membership	   */
#define	IP_DROP_MEMBERSHIP	0x14	/* drop an IP group membership	   */

#define	IP_DEFAULT_MULTICAST_TTL  1	/* normally limit m'casts to 1 hop */
#define	IP_DEFAULT_MULTICAST_LOOP 1	/* normally hear sends if a member */

/*
 * Argument structure for IP_ADD_MEMBERSHIP and IP_DROP_MEMBERSHIP.
 */
struct ip_mreq {
	struct in_addr	imr_multiaddr;	/* IP multicast address of group */
	struct in_addr	imr_interface;	/* local IP address of interface */
};

/*
 * Macros for number representation conversion now live in <sys/byteorder.h>.
 */

#ifdef	_KERNEL
struct	in_addr in_makeaddr();
u_long	in_netof(), in_lnaof();

/*
 * Useful defines, should be in stream.h
 */
#define	msgbavail(bp)	((bp)->b_datap->db_lim - (bp)->b_wptr)
#define	msgblen(bp)	((bp)->b_wptr - (bp)->b_rptr)

#endif	/* _KERNEL */

#ifdef	__cplusplus
}
#endif

#endif	/* _NETINET_IN_H */
