/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)yp_b.h	1.10	95/09/26 SMI"        /* SVr4.0 1.1   */


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*	PROPRIETARY NOTICE (Combined)
*
* This source code is unpublished proprietary information
* constituting, or derived under license from AT&T's UNIX(r) System V.
* In addition, portions of such source code were derived from Berkeley
* 4.3 BSD under license from the Regents of the University of
* California.
*
*
*
*	Copyright Notice
*
* Notice of copyright on this source code product does not indicate 
*  publication.
*
*	(c) 1986,1987,1988,1989,1990  Sun Microsystems, Inc
*	(c) 1983,1984,1985,1986,1987,1988,1989  AT&T.
*          All rights reserved.
*/ 

#include <rpc/types.h>
#include "netconfig.h"
#include <stdio.h>
#include <synch.h>
#include <netdb.h>

extern bool_t xdr_netconfig();

#define	BINDING "/var/yp/binding"
#define	YPSETLOCAL 3

struct dom_binding {
	struct dom_binding *dom_pnext;
	char *dom_domain;
	struct ypbind_binding *dom_binding;
	CLIENT *dom_client;
	int cache_bad;
	int fd;		/* fd in dom_client */
	dev_t rdev;	/* device id of fd */
	int ref_count;	/* number of threads using this structure */
	int need_free;	/* if true, this structure needs to be freed */
	mutex_t server_name_lock;    /* protects server name in dom_binding */
};

/* Following structure is used only by ypbind */

struct domain {
	struct domain *dom_pnext;
	char	*dom_name;
	bool_t dom_boundp;
	unsigned short dom_vers;	/* only YPVERS */
	unsigned long	dom_error;
	CLIENT * ping_clnt;
	struct ypbind_binding *dom_binding;
	int	dom_report_success;	/* Controls msg to /dev/console*/
	int	dom_broadcaster_pid;
	int	bindfile;		/* File with binding info in it */
	int 	broadcaster_fd;
	FILE    *broadcaster_pipe;	/* to get answer from locater */
	XDR	broadcaster_xdr;	/* xdr for pipe */
	struct timeval lastping;	/* info to avoid a ping storm */
};

enum ypbind_resptype {
	YPBIND_SUCC_VAL = 1,
	YPBIND_FAIL_VAL = 2
};
typedef enum ypbind_resptype ypbind_resptype;
bool_t xdr_ypbind_resptype();
#define	YPBIND_ERR_ERR 1		/* Internal error */
#define	YPBIND_ERR_NOSERV 2		/* No bound server for passed domain */
#define	YPBIND_ERR_RESC 3		/* System resource allocation failure */
#define	YPBIND_ERR_NODOMAIN 4		/* Domain doesn't exist */

/* Following struct is used only by ypwhich and yppoll */

struct ypbind_domain {
	char *ypbind_domainname;
	long ypbind_vers;
};
typedef struct ypbind_domain ypbind_domain;
bool_t xdr_ypbind_domain();

/*
 * This structure is used to store information about the server
 * Returned by ypbind to the libnsl/yp clients to contact ypserv.
 * Also used by ypxfr.
 */

struct ypbind_binding {
	struct netconfig *ypbind_nconf;
	struct netbuf *ypbind_svcaddr;
	char *ypbind_servername;
	long ypbind_hi_vers;
	long ypbind_lo_vers;
};
typedef struct ypbind_binding ypbind_binding;
bool_t xdr_ypbind_binding();

struct ypbind_resp {
	ypbind_resptype ypbind_status;
	union {
		u_long ypbind_error;
		struct ypbind_binding *ypbind_bindinfo;
	} ypbind_resp_u;
};
typedef struct ypbind_resp ypbind_resp;
bool_t xdr_ypbind_resp();

struct ypbind_setdom {
	char *ypsetdom_domain;
	struct ypbind_binding *ypsetdom_bindinfo;
};
typedef struct ypbind_setdom ypbind_setdom;
bool_t xdr_ypbind_setdom();

#define	YPBINDPROG ((u_long)100007)
#define	YPBINDVERS ((u_long)3)
#define	YPBINDPROC_NULL ((u_long)0)
extern void *ypbindproc_null_3();
#define	YPBINDPROC_DOMAIN ((u_long)1)
extern ypbind_resp *ypbindproc_domain_3();
#define	YPBINDPROC_SETDOM ((u_long)2)
extern void *ypbindproc_setdom_3();
