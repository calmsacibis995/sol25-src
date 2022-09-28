/*
 * Copyright (c) 1986 - 1991 by Sun Microsystems, Inc.
 */

/*
 * auth_sys.h, Protocol for UNIX style authentication parameters for RPC
 *
 */

#ifndef	_RPC_AUTH_SYS_H
#define	_RPC_AUTH_SYS_H

#pragma ident	"@(#)auth_sys.h	1.14	95/01/26 SMI"

/*
 * The system is very weak.  The client uses no encryption for  it
 * credentials and only sends null verifiers.  The server sends backs
 * null verifiers or optionally a verifier that suggests a new short hand
 * for the credentials.
 */

#ifdef	__cplusplus
extern "C" {
#endif

/* The machine name is part of a credential; it may not exceed 255 bytes */
#define	 MAX_MACHINE_NAME 255

/* gids compose part of a credential; there may not be more than 16 of them */
#define	 NGRPS 16

/*
 * "sys" (Old UNIX) style credentials.
 */
struct authsys_parms {
	u_long	 aup_time;
	char	*aup_machname;
	uid_t	 aup_uid;
	gid_t	 aup_gid;
	u_int	 aup_len;
	gid_t	*aup_gids;
};
/* For backward compatibility */
#define	 authunix_parms authsys_parms

#ifdef __STDC__
extern bool_t xdr_authsys_parms(XDR *, struct authsys_parms *);
#else
extern bool_t xdr_authsys_parms();
#endif


/* For backward compatibility */
#define	xdr_authunix_parms(xdrs, p) xdr_authsys_parms(xdrs, p)

/*
 * If a response verifier has flavor AUTH_SHORT, then the body of
 * the response verifier encapsulates the following structure;
 * again it is serialized in the obvious fashion.
 */
struct short_hand_verf {
	struct opaque_auth new_cred;
};

#ifdef _KERNEL
#ifdef __STDC__
extern bool_t	xdr_authkern(XDR *);
extern enum auth_stat _svcauth_unix(struct svc_req *, struct rpc_msg *);
extern enum auth_stat _svcauth_short(struct svc_req *, struct rpc_msg *);
#else
extern bool_t	xdr_authkern();
extern enum auth_stat _svcauth_unix();
extern enum auth_stat _svcauth_short();
#endif
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* !_RPC_AUTH_SYS_H */
