/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifndef	_SYS_FS_NAMENODE_H
#define	_SYS_FS_NAMENODE_H

#pragma ident	"@(#)namenode.h	1.14	93/05/10 SMI"	/* SVr4.0 1.4	*/

#include <sys/vnode.h>

#ifdef	__cplusplus
extern "C" {
#endif

/*
 * This structure is used to pass a file descriptor from user
 * level to the kernel. It is first used by fattach() and then
 * be NAMEFS.
 */
struct namefd {
	int fd;
};

/*
 * Each NAMEFS object is identified by a struct namenode/vnode pair.
 */
struct namenode {
	struct vnode    nm_vnode;	/* represents mounted file desc. */
	ushort		nm_flag;	/* flags defined below */
	struct vattr    nm_vattr;	/* attributes of mounted file desc. */
	struct vnode	*nm_filevp;	/* file desc. prior to mounting */
	struct file	*nm_filep;	/* file pointer of nm_filevp */
	struct vnode	*nm_mountpt;	/* mount point prior to mounting */
	struct namenode *nm_nextp;	/* next link in the linked list */
	struct namenode *nm_backp;	/* back link in linked list */
	kmutex_t	nm_lock;		/* per namenode lock */
};

/*
 * nm_lock protects nm_flag and nm_vattr.  nm_flag is not used yet.
 */
/*
 * Valid flags for namenodes.
 */
#define	NMLOCK		01	/* the namenode is locked */
#define	NMWANT		02	/* a process wants the namenode */


/*
 * Macros to convert a vnode to a namenode, and vice versa.
 */
#define	VTONM(vp) ((struct namenode *)((vp)->v_data))
#define	NMTOV(nm) (&(nm)->nm_vnode)

#if defined(_KERNEL)
extern	struct vfsops nmvfsops;

struct vfssw;
struct mounta;

int	nameinit(struct vfssw *, int);
int	nm_unmountall(struct vnode *, struct cred *);
void	nameinsert(struct namenode *);
void	nameremove(struct namenode *);
void	nmclearid(struct namenode *);
u_short	nmgetid(u_short);
struct namenode *namefind(struct vnode *, struct vnode *);
extern struct vnodeops nm_vnodeops;
extern kmutex_t ntable_lock;
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_FS_NAMENODE_H */
