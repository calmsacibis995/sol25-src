/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)svc_auth.c	1.12	95/01/26 SMI"	/* SVr4.0 1.3	*/

/*
 *  		PROPRIETARY NOTICE (Combined)
 *
 *  This source code is unpublished proprietary information
 *  constituting, or derived under license from AT&T's Unix(r) System V.
 *  In addition, portions of such source code were derived from Berkeley
 *  4.3 BSD under license from the Regents of the University of
 *  California.
 *
 *
 *
 *  		Copyright Notice
 *
 *  Notice of copyright on this source code product does not indicate
 *  publication.
 *
 *  	(c) 1986, 1987, 1988, 1989  Sun Microsystems, Inc.
 *  	(c) 1983, 1984, 1985, 1986, 1987, 1988, 1989  AT&T.
 *		  All rights reserved.
 */

/*
 * svc_auth.c, Server-side rpc authenticator interface.
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/stream.h>
#include <sys/stropts.h>
#include <sys/strsubr.h>
#include <sys/tiuser.h>
#include <sys/tihdr.h>
#include <sys/t_kuser.h>

#include <rpc/types.h>
#include <rpc/xdr.h>
#include <rpc/auth.h>
#include <rpc/clnt.h>
#include <rpc/rpc_msg.h>
#include <rpc/auth_sys.h>
#include <rpc/auth_des.h>
#include <rpc/auth_kerb.h>
#include <rpc/svc.h>
#include <rpc/svc_auth.h>

/*
 * svcauthsw is the bdevsw of server side authentication.
 *
 * Server side authenticators are called from authenticate by
 * using the client auth struct flavor field to index into svcauthsw.
 * The server auth flavors must implement a routine that looks
 * like:
 *
 *	enum auth_stat
 *	flavorx_auth(rqst, msg)
 *		register struct svc_req *rqst;
 *		register struct rpc_msg *msg;
 *
 */

static enum auth_stat _svcauth_null(struct svc_req *, struct rpc_msg *);

/*
 * The call rpc message, msg has been obtained from the wire.  The msg contains
 * the raw form of credentials and verifiers.  authenticate returns AUTH_OK
 * if the msg is successfully authenticated.  If AUTH_OK then the routine also
 * does the following things:
 * set rqst->rq_xprt->verf to the appropriate response verifier;
 * sets rqst->rq_client_cred to the "cooked" form of the credentials.
 *
 * NB: rqst->rq_cxprt->verf must be pre-alloctaed;
 * its length is set appropriately.
 *
 * The caller still owns and is responsible for msg->u.cmb.cred and
 * msg->u.cmb.verf.  The authentication system retains ownership of
 * rqst->rq_client_cred, the cooked credentials.
 *
 * There is an assumption that any flavour less than AUTH_NULL is
 * invalid.
 */
enum auth_stat
_authenticate(struct svc_req *rqst, struct rpc_msg *msg)
{
	register int cred_flavor;

	rqst->rq_cred = msg->rm_call.cb_cred;
	rqst->rq_xprt->xp_verf.oa_flavor = _null_auth.oa_flavor;
	rqst->rq_xprt->xp_verf.oa_length = 0;
	cred_flavor = rqst->rq_cred.oa_flavor;
	switch (cred_flavor) {
	case AUTH_NULL:
		return (_svcauth_null(rqst, msg));
	case AUTH_UNIX:
		return (_svcauth_unix(rqst, msg));
	case AUTH_SHORT:
		return (_svcauth_short(rqst, msg));
	case AUTH_DES:
		return (_svcauth_des(rqst, msg));
	case AUTH_KERB:
		return (_svcauth_kerb(rqst, msg));
	}
	return (AUTH_REJECTEDCRED);
}

/* ARGSUSED */
static enum auth_stat
_svcauth_null(struct svc_req *rqst, struct rpc_msg *msg)
{

	return (AUTH_OK);
}
