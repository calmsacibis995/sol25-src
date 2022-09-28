 /*
  * Copyright (c) 1992 Sun Microsystems, Inc.  All Rights Reserved.
  */

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#pragma ident "@(#)systable.c	1.25	95/07/14 SMI"	/* SVr4.0 1.8	*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/signal.h>
#include <sys/fault.h>
#include <sys/syscall.h>
#ifdef SYS_kaio
#include <sys/aio.h>
#endif
#include "pcontrol.h"
#include "ramdata.h"
#include "systable.h"
#include "print.h"
#include "proto.h"

/* tables of information about system calls - read-only data */


static	CONST	char * CONST	errcode[] = {	/* error code names */
	 NULL,		/*  0 */
	"EPERM",	/*  1 */
	"ENOENT",	/*  2 */
	"ESRCH",	/*  3 */
	"EINTR",	/*  4 */
	"EIO",		/*  5 */
	"ENXIO",	/*  6 */
	"E2BIG",	/*  7 */
	"ENOEXEC",	/*  8 */
	"EBADF",	/*  9 */
	"ECHILD",	/* 10 */
	"EAGAIN",	/* 11 */
	"ENOMEM",	/* 12 */
	"EACCES",	/* 13 */
	"EFAULT",	/* 14 */
	"ENOTBLK",	/* 15 */
	"EBUSY",	/* 16 */
	"EEXIST",	/* 17 */
	"EXDEV",	/* 18 */
	"ENODEV",	/* 19 */
	"ENOTDIR",	/* 20 */
	"EISDIR",	/* 21 */
	"EINVAL",	/* 22 */
	"ENFILE",	/* 23 */
	"EMFILE",	/* 24 */
	"ENOTTY",	/* 25 */
	"ETXTBSY",	/* 26 */
	"EFBIG",	/* 27 */
	"ENOSPC",	/* 28 */
	"ESPIPE",	/* 29 */
	"EROFS",	/* 30 */
	"EMLINK",	/* 31 */
	"EPIPE",	/* 32 */
	"EDOM",		/* 33 */
	"ERANGE",	/* 34 */
	"ENOMSG",	/* 35 */
	"EIDRM",	/* 36 */
	"ECHRNG",	/* 37 */
	"EL2NSYNC",	/* 38 */
	"EL3HLT",	/* 39 */
	"EL3RST",	/* 40 */
	"ELNRNG",	/* 41 */
	"EUNATCH",	/* 42 */
	"ENOCSI",	/* 43 */
	"EL2HLT",	/* 44 */
	"EDEADLK",	/* 45 */
	"ENOLCK",	/* 46 */
	 NULL,		/* 47 */
	 NULL,		/* 48 */
	 NULL,		/* 49 */
	"EBADE",	/* 50 */
	"EBADR",	/* 51 */
	"EXFULL",	/* 52 */
	"ENOANO",	/* 53 */
	"EBADRQC",	/* 54 */
	"EBADSLT",	/* 55 */
	"EDEADLOCK",	/* 56 */
	"EBFONT",	/* 57 */
	 NULL,		/* 58 */
	 NULL,		/* 59 */
	"ENOSTR",	/* 60 */
	"ENODATA",	/* 61 */
	"ETIME",	/* 62 */
	"ENOSR",	/* 63 */
	"ENONET",	/* 64 */
	"ENOPKG",	/* 65 */
	"EREMOTE",	/* 66 */
	"ENOLINK",	/* 67 */
	"EADV",		/* 68 */
	"ESRMNT",	/* 69 */
	"ECOMM",	/* 70 */
	"EPROTO",	/* 71 */
	 NULL,		/* 72 */
	 NULL,		/* 73 */
	"EMULTIHOP",	/* 74 */
	 NULL,		/* 75 */
	 NULL,		/* 76 */
	"EBADMSG",	/* 77 */
	"ENAMETOOLONG",	/* 78 */
	"EOVERFLOW",	/* 79 */
	"ENOTUNIQ",	/* 80 */
	"EBADFD",	/* 81 */
	"EREMCHG",	/* 82 */
	"ELIBACC",	/* 83 */
	"ELIBBAD",	/* 84 */
	"ELIBSCN",	/* 85 */
	"ELIBMAX",	/* 86 */
	"ELIBEXEC",	/* 87 */
	"EILSEQ",	/* 88 */
	"ENOSYS",	/* 89 */
	"ELOOP",	/* 90 */
	"ERESTART",	/* 91 */
	"ESTRPIPE",	/* 92 */
	"ENOTEMPTY",	/* 93 */
	"EUSERS",	/* 94 */
	"ENOTSOCK",	/* 95 */
	"EDESTADDRREQ",	/* 96 */
	"EMSGSIZE",	/* 97 */
	"EPROTOTYPE",	/* 98 */
	"ENOPROTOOPT",	/* 99 */
	 NULL,		/* 100 */
	 NULL,		/* 101 */
	 NULL,		/* 102 */
	 NULL,		/* 103 */
	 NULL,		/* 104 */
	 NULL,		/* 105 */
	 NULL,		/* 106 */
	 NULL,		/* 107 */
	 NULL,		/* 108 */
	 NULL,		/* 109 */
	 NULL,		/* 110 */
	 NULL,		/* 111 */
	 NULL,		/* 112 */
	 NULL,		/* 113 */
	 NULL,		/* 114 */
	 NULL,		/* 115 */
	 NULL,		/* 116 */
	 NULL,		/* 117 */
	 NULL,		/* 118 */
	 NULL,		/* 119 */
	"EPROTONOSUPPORT",	/* 120 */
	"ESOCKTNOSUPPORT",	/* 121 */
	"EOPNOTSUPP",	/* 122 */
	"EPFNOSUPPORT",	/* 123 */
	"EAFNOSUPPORT",	/* 124 */
	"EADDRINUSE",	/* 125 */
	"EADDRNOTAVAIL",/* 126 */
	"ENETDOWN",	/* 127 */
	"ENETUNREACH",	/* 128 */
	"ENETRESET",	/* 129 */
	"ECONNABORTED",	/* 130 */
	"ECONNRESET",	/* 131 */
	"ENOBUFS",	/* 132 */
	"EISCONN",	/* 133 */
	"ENOTCONN",	/* 134 */
	"EUCLEAN",	/* 135 */
	 NULL,		/* 136 */
	"ENOTNAM",	/* 137 */
	"ENAVAIL",	/* 138 */
	"EISNAM",	/* 139 */
	"EREMOTEIO",	/* 140 */
	"EINIT",	/* 141 */
	"EREMDEV",	/* 142 */
	"ESHUTDOWN",	/* 143 */
	"ETOOMANYREFS",	/* 144 */
	"ETIMEDOUT",	/* 145 */
	"ECONNREFUSED",	/* 146 */
	"EHOSTDOWN",	/* 147 */
	"EHOSTUNREACH",	/* 148 */
	"EALREADY",	/* 149 */
	"EINPROGRESS",	/* 150 */
	"ESTALE"	/* 151 */

};

#define	NERRCODE	(sizeof(errcode)/sizeof(char *))


#define	NIBASE	200
static	CONST	char * CONST	nicode[] = {	/* 3BNET error code names */
	"EBADADDR",		/* 200 */
	"EBADCONFIG",		/* 201 */
	"ENOTCONFIG",		/* 202 */
	"EOPENFAILED",		/* 203 */
	"ENOCIRCUIT",		/* 204 */
	"EHLPFAULT",		/* 205 */
	"EPORTNOTAVAIL",	/* 206 */
	"ENETNOTAVAIL",		/* 207 */
	"EDRIVERFAULT",		/* 208 */
	"EDEVICEFAULT",		/* 209 */
	"ENETFAULT",		/* 210 */
	"EBADPACKET",		/* 211 */
	"EDEVICERESET",		/* 212 */
};

#define	NNICODE		(sizeof(nicode)/sizeof(char *))


CONST char *
errname(err)		/* return the error code name (NULL if none) */
register int err;
{
	register CONST char * ename = NULL;

	if (err >= 0 && err < NERRCODE)
		ename = errcode[err];
	else if (err >= NIBASE && err < NIBASE+NNICODE)
		ename = nicode[err-NIBASE];

	return ename;
}


CONST	struct systable systable[] = {
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /*  0 */
 {"_exit",	1, DEC, NOV, DEC},				      /*  1 */
 {"fork",	0, DEC, NOV},					      /*  2 */
 {"read",	3, DEC, NOV, DEC, IOB, DEC},			      /*  3 */
 {"write",	3, DEC, NOV, DEC, IOB, DEC},			      /*  4 */
 {"open",	3, DEC, NOV, STG, OPN, OCT},			      /*  5 */
 {"close",	1, DEC, NOV, DEC},				      /*  6 */
 {"wait",	0, DEC, HHX},					      /*  7 */
 {"creat",	2, DEC, NOV, STG, OCT},				      /*  8 */
 {"link",	2, DEC, NOV, STG, STG},				      /*  9 */
 {"unlink",	1, DEC, NOV, STG},				      /* 10 */
 {"exec",	2, DEC, NOV, STG, DEC},				      /* 11 */
 {"chdir",	1, DEC, NOV, STG},				      /* 12 */
 {"time",	0, DEC, NOV},					      /* 13 */
 {"mknod",	3, DEC, NOV, STG, OCT, HEX},			      /* 14 */
 {"chmod",	2, DEC, NOV, STG, OCT},				      /* 15 */
 {"chown",	3, DEC, NOV, STG, DEC, DEC},			      /* 16 */
 {"brk",	1, DEC, NOV, HEX},				      /* 17 */
 {"stat",	2, DEC, NOV, STG, HEX},				      /* 18 */
 {"lseek",	3, DEC, NOV, DEC, DEX, WHN},			      /* 19 */
 {"getpid",	0, DEC, DEC},					      /* 20 */
 {"mount",	6, DEC, NOV, STG, STG, MTF, MFT, HEX, DEC},	      /* 21 */
 {"umount",	1, DEC, NOV, STG},				      /* 22 */
 {"setuid",	1, DEC, NOV, DEC},				      /* 23 */
 {"getuid",	0, DEC, DEC},					      /* 24 */
 {"stime",	1, DEC, NOV, DEC},				      /* 25 */
 {"ptrace",	4, HEX, NOV, DEC, DEC, HEX, HEX},		      /* 26 */
 {"alarm",	1, DEC, NOV, DEC},				      /* 27 */
 {"fstat",	2, DEC, NOV, DEC, HEX},				      /* 28 */
 {"pause",	0, DEC, NOV},					      /* 29 */
 {"utime",	2, DEC, NOV, STG, HEX},				      /* 30 */
 {"stty",	2, DEC, NOV, DEC, DEC},				      /* 31 */
 {"gtty",	2, DEC, NOV, DEC, DEC},				      /* 32 */
 {"access",	2, DEC, NOV, STG, DEC},				      /* 33 */
 {"nice",	1, DEC, NOV, DEC},				      /* 34 */
 {"statfs",	4, DEC, NOV, STG, HEX, DEC, DEC},		      /* 35 */
 {"sync",	0, DEC, NOV},					      /* 36 */
 {"kill",	2, DEC, NOV, DEC, SIG},				      /* 37 */
 {"fstatfs",	4, DEC, NOV, DEC, HEX, DEC, DEC},		      /* 38 */
 {"pgrpsys",	3, DEC, NOV, DEC, DEC, DEC},			      /* 39 */
 {"xenix",	5, DEC, NOV, DEC, HEX, HEX, HEX, HEX},		      /* 40 */
 {"dup",	1, DEC, NOV, DEC},				      /* 41 */
 {"pipe",	0, DEC, DEC},					      /* 42 */
 {"times",	1, DEC, NOV, HEX},				      /* 43 */
 {"profil",	4, DEC, NOV, HEX, DEC, HEX, OCT},		      /* 44 */
 {"plock",	1, DEC, NOV, PLK},				      /* 45 */
 {"setgid",	1, DEC, NOV, DEC},				      /* 46 */
 {"getgid",	0, DEC, DEC},					      /* 47 */
 {"signal",	2, HEX, NOV, SIG, ACT},				      /* 48 */
 {"msgsys",	6, DEC, NOV, DEC, DEC, DEC, DEC, DEC, DEC},	      /* 49 */
 #if defined(i386)
 {"sysi86",	4, HEX, NOV, S86, HEX, HEX, HEX, DEC, DEC},	      /* 50 */
 #else /* !386 */
 {"sys3b",	4, HHX, NOV, S3B, HEX, HEX, HEX},		      /* 50 */
 #endif /* !386 */
 {"acct",	1, DEC, NOV, STG},				      /* 51 */
 {"shmsys",	4, DEC, NOV, DEC, HEX, HEX, HEX},		      /* 52 */
 {"semsys",	5, DEC, NOV, DEC, HEX, HEX, HEX, HEX},		      /* 53 */
 {"ioctl",	3, DEC, NOV, DEC, IOC, IOA},			      /* 54 */
 {"uadmin",	3, DEC, NOV, DEC, DEC, DEC},			      /* 55 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 56 */
 {"utssys",	4, DEC, NOV, HEX, DEC, UTS, HEX},		      /* 57 */
 {"fdsync",	2, DEC, NOV, DEC, OPN},				      /* 58 */
 {"execve",	3, DEC, NOV, STG, HEX, HEX},			      /* 59 */
 {"umask",	1, OCT, NOV, OCT},				      /* 60 */
 {"chroot",	1, DEC, NOV, STG},				      /* 61 */
 {"fcntl",	3, DEC, NOV, DEC, FCN, HEX},			      /* 62 */
 {"ulimit",	2, DEX, NOV, ULM, DEC},				      /* 63 */

	/* The following 6 entries were reserved for the UNIX PC */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 64 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 65 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 66 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 67 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 68 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 69 */

	/* Obsolete RFS-specific entries */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 70 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 71 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 72 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 73 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 74 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 75 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 76 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 77 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 78 */

 {"rmdir",	1, DEC, NOV, STG},				      /* 79 */
 {"mkdir",	2, DEC, NOV, STG, OCT},				      /* 80 */
 {"getdents",	3, DEC, NOV, DEC, HEX, DEC},			      /* 81 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 82 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 83 */
 {"sysfs",	3, DEC, NOV, SFS, DEX, DEX},			      /* 84 */
 {"getmsg",	4, DEC, NOV, DEC, HEX, HEX, HEX},		      /* 85 */
 {"putmsg",	4, DEC, NOV, DEC, HEX, HEX, SMF},		      /* 86 */
 {"poll",	3, DEC, NOV, HEX, DEC, DEC},			      /* 87 */
 {"lstat",	2, DEC, NOV, STG, HEX},				      /* 88 */
 {"symlink",	2, DEC, NOV, STG, STG},				      /* 89 */
 {"readlink",	3, DEC, NOV, STG, RLK, DEC},			      /* 90 */
 {"setgroups",	2, DEC, NOV, DEC, HEX},				      /* 91 */
 {"getgroups",	2, DEC, NOV, DEC, HEX},				      /* 92 */
 {"fchmod",	2, DEC, NOV, DEC, OCT},				      /* 93 */
 {"fchown",	3, DEC, NOV, DEC, DEC, DEC},			      /* 94 */
 {"sigprocmask",3, DEC, NOV, SPM, HEX, HEX},			      /* 95 */
 {"sigsuspend",	1, DEC, NOV, HEX},				      /* 96 */
 {"sigaltstack",2, DEC, NOV, HEX, HEX},				      /* 97 */
 {"sigaction",	3, DEC, NOV, SIG, HEX, HEX},			      /* 98 */
 {"sigpending",	2, DEC, NOV, DEC, HEX},				      /* 99 */
 {"context",	2, DEC, NOV, DEC, HEX},				      /* 100 */
 {"evsys",	3, DEC, NOV, DEC, DEC, HEX},			      /* 101 */
 {"evtrapret",	0, DEC, NOV},					      /* 102 */
 {"statvfs",	2, DEC, NOV, STG, HEX},				      /* 103 */
 {"fstatvfs",	2, DEC, NOV, DEC, HEX},				      /* 104 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 105 */
 {"nfssys",	2, DEC, NOV, DEC, HEX},				      /* 106 */
 {"waitid",	4, DEC, NOV, IDT, DEC, HEX, WOP},		      /* 107 */
 {"sigsendsys",	2, DEC, NOV, HEX, SIG},				      /* 108 */
 {"hrtsys",	5, DEC, NOV, DEC, HEX, HEX, HEX, HEX},		      /* 109 */
 {"acancel",	3, DEC, NOV, DEC, HEX, DEC},			      /* 110 */
 {"async",	3, DEC, NOV, DEC, HEX, DEC},			      /* 111 */
 {"priocntlsys",4, DEC, NOV, DEC, HEX, DEC, HEX},		      /* 112 */
 {"pathconf",	2, DEC, NOV, STG, PTC},				      /* 113 */
 {"mincore",	3, DEC, NOV, HEX, DEC, HEX},			      /* 114 */
 {"mmap",	6, HEX, NOV, HEX, DEC, MPR, MTY, DEC, DEC},	      /* 115 */
 {"mprotect",	3, DEC, NOV, HEX, DEC, MPR},			      /* 116 */
 {"munmap",	2, DEC, NOV, HEX, DEC},				      /* 117 */
 {"fpathconf",	2, DEC, NOV, DEC, PTC},				      /* 118 */
 {"vfork",	0, DEC, NOV},					      /* 119 */
 {"fchdir",	1, DEC, NOV, DEC},				      /* 120 */
 {"readv",	3, DEC, NOV, DEC, HEX, DEC},			      /* 121 */
 {"writev",	3, DEC, NOV, DEC, HEX, DEC},			      /* 122 */
 {"xstat",	3, DEC, NOV, DEC, STG, HEX},			      /* 123 */
 {"lxstat",	3, DEC, NOV, DEC, STG, HEX},			      /* 124 */
 {"fxstat",	3, DEC, NOV, DEC, DEC, HEX},			      /* 125 */
 {"xmknod",	4, DEC, NOV, DEC, STG, OCT, HEX},		      /* 126 */
 {"clocal",	5, HEX, HEX, DEC, HEX, HEX, HEX, HEX},		      /* 127 */
 {"setrlimit",	2, DEC, NOV, RLM, HEX},				      /* 128 */
 {"getrlimit",	2, DEC, NOV, RLM, HEX},				      /* 129 */
 {"lchown",	3, DEC, NOV, STG, DEC, DEC},			      /* 130 */
 {"memcntl",	6, DEC, NOV, HEX, DEC, MCF, MC4, MC5, DEC},	      /* 131 */
 {"getpmsg",	5, DEC, NOV, DEC, HEX, HEX, HEX, HEX},		      /* 132 */
 {"putpmsg",	5, DEC, NOV, DEC, HEX, HEX, DEC, HHX},		      /* 133 */
 {"rename",	2, DEC, NOV, STG, STG},				      /* 134 */
 {"uname",	1, DEC, NOV, HEX},				      /* 135 */
 {"setegid",	1, DEC, NOV, DEC},				      /* 136 */
 {"sysconfig",	1, DEC, NOV, CNF},				      /* 137 */
 {"adjtime",	2, DEC, NOV, HEX, HEX},				      /* 138 */
 {"systeminfo",	3, DEC, NOV, INF, RST, DEC},			      /* 139 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 140 */
 {"seteuid",	1, DEC, NOV, DEC},				      /* 141 */
 {"vtrace",	3, DEC, NOV, VTR, HEX, HEX},			      /* 142 */
 {"fork1",	0, DEC, NOV},					      /* 143 */
 {"sigtimedwait",3,DEC, NOV, HEX, HEX, HEX},			      /* 144 */
 {"lwp_info",	1, DEC, NOV, HEX},				      /* 145 */
 {"yield",	0, DEC, NOV},					      /* 146 */
 {"lwp_sema_p",	1, DEC, NOV, HEX},				      /* 147 */
 {"lwp_sema_v",	1, DEC, NOV, HEX},				      /* 148 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 149 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 150 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 151 */
 {"modctl",	4, DEC, NOV, MOD, HEX, HEX, HEX},		      /* 152 */
 {"fchroot",	1, DEC, NOV, DEC},				      /* 153 */
 {"utimes",	2, DEC, NOV, STG, HEX},				      /* 154 */
 {"vhangup",	0, DEC, NOV},					      /* 155 */
 {"gettimeofday",1,DEC, NOV, HEX},				      /* 156 */
 {"getitimer",	2, DEC, NOV, ITM, HEX},				      /* 157 */
 {"setitimer",	3, DEC, NOV, ITM, HEX, HEX},			      /* 158 */
 {"lwp_create",	3, DEC, NOV, HEX, LWF, HEX},			      /* 159 */
 {"lwp_exit",	0, DEC, NOV},					      /* 160 */
 {"lwp_suspend",1, DEC, NOV, DEC},				      /* 161 */
 {"lwp_continue",1,DEC, NOV, DEC},				      /* 162 */
 {"lwp_kill",	2, DEC, NOV, DEC, SIG},				      /* 163 */
 {"lwp_self",	0, DEC, NOV},					      /* 164 */
 {"lwp_setprivate",1,DEC,NOV,HEX},				      /* 165 */
 {"lwp_getprivate",0,HEX,NOV},					      /* 166 */
 {"lwp_wait",	2, DEC, NOV, DEC, HEX},				      /* 167 */
 {"lwp_mutex_unlock",1,DEC,NOV,HEX},				      /* 168 */
 {"lwp_mutex_lock",1,DEC,NOV,HEX},				      /* 169 */
 {"lwp_cond_wait",3,DEC,NOV, HEX, HEX, HEX},			      /* 170 */
 {"lwp_cond_signal",1,DEC,NOV,HEX},				      /* 171 */
 {"lwp_cond_broadcast",1,DEC,NOV,HEX},				      /* 172 */
 {"pread",	4, DEC, NOV, DEC, IOB, DEC, DEX},		      /* 173 */
 {"pwrite",	4, DEC, NOV, DEC, IOB, DEC, DEX},		      /* 174 */
 {"llseek",	4, LLO, NOV, DEC, LLO, HID, WHN},		      /* 175 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 176 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 177 */
 {"kaio",	7, DEC, NOV, AIO, HEX, HEX, HEX, HEX, HEX, HEX},      /* 178 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 179 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 180 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 181 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 182 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 183 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 184 */
 {"acl",	4, DEC, NOV, STG, ACL, DEC, HEX},		      /* 185 */
 {"auditsys",	4, DEC, NOV, AUD, HEX, HEX, HEX},		      /* 186 */
 {"processor_bind", 4, DEC, NOV, IDT, DEC, DEC, HEX},		      /* 187 */
 {"processor_info", 2, DEC, NOV, DEC, HEX},			      /* 188 */
 {"p_online",	2, DEC, NOV, DEC, DEC},				      /* 189 */
 {"sigqueue",	3, DEC, NOV, DEC, SIG, HEX},			      /* 190 */
 {"clock_gettime",2,DEC,NOV, DEC, HEX},				      /* 191 */
 {"clock_settime",2,DEC,NOV, DEC, HEX},				      /* 192 */
 {"clock_getres",2, DEC, NOV, DEC, HEX},			      /* 193 */
 {"timer_create",3, DEC, NOV, DEC, HEX, HEX},			      /* 194 */
 {"timer_delete",1, DEC, NOV, DEC},				      /* 195 */
 {"timer_settime",4,DEC, NOV, DEC, DEC, HEX, HEX},		      /* 196 */
 {"timer_gettime",2,DEC, NOV, DEC, HEX},			      /* 197 */
 {"timer_getoverrun",1,DEC,NOV, DEC},				      /* 198 */
 {"nanosleep",	2, DEC, NOV, HEX, HEX},				      /* 199 */
 {"facl",	4, DEC, NOV, DEC, ACL, DEC, HEX},		      /* 200 */
 {"door",	6, DEC, NOV, DEC, HEX, HEX, HEX, HEX, HEX},	      /* 201 */
 {"setreuid",	2, DEC, NOV, DEC, DEC},				      /* 202 */
 {"setregid",	2, DEC, NOV, DEC, DEC},				      /* 203 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 204 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 205 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 206 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 207 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 208 */
 { NULL,	8, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX, HEX}, /* 209 */
 {"signotifywait", 0, DEC, NOV},			      	      /* 210 */
 {"lwp_sigredirect", 2, DEC, NOV, DEC, SIG},			      /* 211 */
 {"lwp_alarm",	1, DEC, NOV, DEC},				      /* 212 */
 { NULL,-1,DEC, NOV},						      /* end */
};

/* SYSEND == max syscall number + 1 */
#define	SYSEND	((sizeof(systable)/sizeof(struct systable))-1)


/* The following are for interpreting syscalls with sub-codes */

static	CONST	struct systable opentable[] = {
 {"open",	2, DEC, NOV, STG, OPN},				      /* 0 */
 {"open",	3, DEC, NOV, STG, OPN, OCT},			      /* 1 */
};
#define	NOPENCODE	(sizeof(opentable)/sizeof(struct systable))

static	CONST	struct systable sigtable[] = {
 {"signal",	2, HEX, NOV, SIG, ACT},				      /* 0 */
 {"sigset",	2, HEX, NOV, SIX, ACT},				      /* 1 */
 {"sighold",	1, HEX, NOV, SIX},				      /* 2 */
 {"sigrelse",	1, HEX, NOV, SIX},				      /* 3 */
 {"sigignore",	1, HEX, NOV, SIX},				      /* 4 */
 {"sigpause",	1, HEX, NOV, SIX},				      /* 5 */
};
#define	NSIGCODE	(sizeof(sigtable)/sizeof(struct systable))

static	CONST	struct systable msgtable[] = {
 {"msgget",	3, DEC, NOV, HID, DEC, MSF},			      /* 0 */
 {"msgctl",	4, DEC, NOV, HID, DEC, MSC, HEX},		      /* 1 */
 {"msgrcv",	6, DEC, NOV, HID, DEC, HEX, DEC, DEC, MSF},	      /* 2 */
 {"msgsnd",	5, DEC, NOV, HID, DEC, HEX, DEC, MSF},		      /* 3 */
};
#define	NMSGCODE	(sizeof(msgtable)/sizeof(struct systable))

static	CONST	struct systable semtable[] = {
 {"semctl",	5, DEC, NOV, HID, DEC, DEC, SMC, DEX},		      /* 0 */
 {"semget",	4, DEC, NOV, HID, DEC, DEC, SEF},		      /* 1 */
 {"semop",	4, DEC, NOV, HID, DEC, HEX, DEC},		      /* 2  */
};
#define	NSEMCODE	(sizeof(semtable)/sizeof(struct systable))

static	CONST	struct systable shmtable[] = {
 {"shmat",	4, HEX, NOV, HID, DEC, DEX, SHF},		      /* 0 */
 {"shmctl",	4, DEC, NOV, HID, DEC, SHC, DEX},		      /* 1 */
 {"shmdt",	2, DEC, NOV, HID, HEX},				      /* 2 */
 {"shmget",	4, DEC, NOV, HID, DEC, DEC, SHF},		      /* 3 */
};
#define	NSHMCODE	(sizeof(shmtable)/sizeof(struct systable))

static	CONST	struct systable pidtable[] = {
 {"getpgrp",	1, DEC, NOV, HID},				      /* 0 */
 {"setpgrp",	1, DEC, NOV, HID},				      /* 1 */
 {"getsid",	2, DEC, NOV, HID, DEC},				      /* 2 */
 {"setsid",	1, DEC, NOV, HID},				      /* 3 */
 {"getpgid",	2, DEC, NOV, HID, DEC},				      /* 4 */
 {"setpgid",	3, DEC, NOV, HID, DEC, DEC},			      /* 5 */
};
#define	NPIDCODE	(sizeof(pidtable)/sizeof(struct systable))

static	CONST	struct systable sfstable[] = {
 {"sysfs",	3, DEC, NOV, SFS, DEX, DEX},			      /* 0 */
 {"sysfs",	2, DEC, NOV, SFS, STG},				      /* 1 */
 {"sysfs",	3, DEC, NOV, SFS, DEC, RST},			      /* 2 */
 {"sysfs",	1, DEC, NOV, SFS},				      /* 3 */
};
#define	NSFSCODE	(sizeof(sfstable)/sizeof(struct systable))

static	CONST	struct systable utstable[] = {
 {"utssys",	3, DEC, NOV, HEX, DEC, UTS},			      /* 0 */
 {"utssys",	4, DEC, NOV, HEX, HEX, HEX, HEX},		      /* err */
 {"utssys",	3, DEC, NOV, HEX, HHX, UTS},			      /* 2 */
 {"utssys",	4, DEC, NOV, STG, FUI, UTS, HEX}		      /* 3 */
};
#define	NUTSCODE	(sizeof(utstable)/sizeof(struct systable))

static	CONST	struct systable sgptable[] = {
 {"sigpending",	2, DEC, NOV, DEC, HEX},				      /* err */
 {"sigpending",	2, DEC, NOV, HID, HEX},				      /* 1 */
 {"sigfillset",	2, DEC, NOV, HID, HEX},				      /* 2 */
};
#define	NSGPCODE	(sizeof(sgptable)/sizeof(struct systable))

static	CONST	struct systable ctxtable[] = {
 {"getcontext",	2, DEC, NOV, HID, HEX},				      /* 0 */
 {"setcontext",	2, DEC, NOV, HID, HEX},				      /* 1 */
};
#define	NCTXCODE	(sizeof(ctxtable)/sizeof(struct systable))

static	CONST	struct systable hrttable[] = {
 {"hrtcntl",	5, DEC, NOV, HID, DEC, DEC, HEX, HEX},		      /* 0 */
 {"hrtalarm",	3, DEC, NOV, HID, HEX, DEC},			      /* 1 */
 {"hrtsleep",	2, DEC, NOV, HID, HEX},				      /* 2 */
 {"hrtcancel",	3, DEC, NOV, HID, HEX, DEC},			      /* 3 */
};
#define	NHRTCODE	(sizeof(hrttable)/sizeof(struct systable))

static	CONST	struct systable vtrtable[] = {
 {"vtrace",	3, DEC, NOV, VTR, DEC, DEC},			      /* 0 */
 {"vtrace",	3, DEC, NOV, VTR, DEC, DEC},			      /* 1 */
 {"vtrace",	3, DEC, NOV, VTR, HEX, DEC},			      /* 2 */
 {"vtrace",	3, DEC, NOV, VTR, HEX},				      /* 3 */
 {"vtrace",	3, DEC, NOV, VTR},				      /* 4 */
 {"vtrace",	3, DEC, NOV, VTR},				      /* 5 */
 {"vtrace",	3, DEC, NOV, VTR},				      /* 6 */
 {"vtrace",	3, DEC, NOV, VTR},				      /* 7 */
 {"vtrace",	3, DEC, NOV, VTR, DEC},				      /* 8 */
 {"vtrace",	3, DEC, NOV, VTR},				      /* 9 */
 {"vtrace",	3, DEC, NOV, VTR, DEC},				      /* 10 */
};
#define	NVTRCODE	(sizeof(vtrtable)/sizeof(struct systable))

static	CONST	struct systable aiotable[] = {
 {"kaio",	7, DEC, NOV, AIO, DEC, HEX, DEC, LLO, HID, HEX},      /* 0 */
 {"kaio",	7, DEC, NOV, AIO, DEC, HEX, DEC, LLO, HID, HEX},      /* 1 */
 {"kaio",	2, DEC, NOV, AIO, HEX},				      /* 2 */
 {"kaio",	2, DEC, NOV, AIO, HEX},				      /* 3 */
 {"kaio",	2, DEC, NOV, AIO, DEC},				      /* 4 */
};
#define	NAIOCODE	(sizeof(aiotable)/sizeof(struct systable))

static	CONST	struct systable xentable[] = {
 {"xenix",	5, DEC, NOV, DEC, HEX, HEX, HEX, HEX},		      /* 0 */
 {"locking",	4, DEC, NOV, HID, DEC, DEC, DEC},		      /* 1 */
 {"creatsem",	3, DEC, NOV, HID, STG, OCT},			      /* 2 */
 {"opensem",	2, DEC, NOV, HID, STG},				      /* 3 */
 {"sigsem",	2, DEC, NOV, HID, DEC},				      /* 4 */
 {"waitsem",	2, DEC, NOV, HID, DEC},				      /* 5 */
 {"nbwaitsem",	2, DEC, NOV, HID, DEC},				      /* 6 */
 {"rdchk",	2, DEC, NOV, HID, DEC},				      /* 7 */
 {"xenix",	5, DEC, NOV, DEC, HEX, HEX, HEX, HEX},		      /* 8 */
 {"xenix",	5, DEC, NOV, DEC, HEX, HEX, HEX, HEX},		      /* 9 */
 {"chsize",	3, DEC, NOV, HID, DEC, DEC},			      /* 10 */
 {"ftime",	2, DEC, NOV, HID, HEX},				      /* 11 */
 {"nap",	2, DEC, NOV, HID, DEC},				      /* 12 */
 {"sdget",	5, DEC, NOV, HID, STG, HHX, DEC, OCT},		      /* 13 */
 {"sdfree",	2, DEC, NOV, HID, HEX},				      /* 14 */
 {"sdenter",	3, DEC, NOV, HID, HEX, HHX},			      /* 15 */
 {"sdleave",	2, DEC, NOV, HID, HEX},				      /* 16 */
 {"sdgetv",	2, DEC, NOV, HID, HEX},				      /* 17 */
 {"sdwaitv",	3, DEC, NOV, HID, HEX, DEC},			      /* 18 */
 {"xenix",	5, DEC, NOV, DEC, HEX, HEX, HEX, HEX},		      /* 19 */
 {"xenix",	5, DEC, NOV, DEC, HEX, HEX, HEX, HEX},		      /* 20 */
 {"xenix",	5, DEC, NOV, DEC, HEX, HEX, HEX, HEX},		      /* 21 */
 {"xenix",	5, DEC, NOV, DEC, HEX, HEX, HEX, HEX},		      /* 22 */
 {"xenix",	5, DEC, NOV, DEC, HEX, HEX, HEX, HEX},		      /* 23 */
 {"xenix",	5, DEC, NOV, DEC, HEX, HEX, HEX, HEX},		      /* 24 */
 {"xenix",	5, DEC, NOV, DEC, HEX, HEX, HEX, HEX},		      /* 25 */
 {"xenix",	5, DEC, NOV, DEC, HEX, HEX, HEX, HEX},		      /* 26 */
 {"xenix",	5, DEC, NOV, DEC, HEX, HEX, HEX, HEX},		      /* 27 */
 {"xenix",	5, DEC, NOV, DEC, HEX, HEX, HEX, HEX},		      /* 28 */
 {"xenix",	5, DEC, NOV, DEC, HEX, HEX, HEX, HEX},		      /* 29 */
 {"xenix",	5, DEC, NOV, DEC, HEX, HEX, HEX, HEX},		      /* 30 */
 {"xenix",	5, DEC, NOV, DEC, HEX, HEX, HEX, HEX},		      /* 31 */
 {"proctl",	4, DEC, NOV, HID, DEC, DEC, HEX},		      /* 32 */
 {"xenix",	5, DEC, NOV, DEC, HEX, HEX, HEX, HEX},		      /* 33 */
 {"xenix",	5, DEC, NOV, DEC, HEX, HEX, HEX, HEX},		      /* 34 */
 {"xenix",	5, DEC, NOV, DEC, HEX, HEX, HEX, HEX},		      /* 35 */
};
#define	NXENCODE	(sizeof(xentable)/sizeof(struct systable))

static	CONST	struct systable doortable[] = {
 {"door_create",3, DEC, NOV, HEX, HEX, HEX},		/* 0 */
 {"door_revoke",1, DEC, NOV, DEC},			/* 1 */
 {"door_info",	2, DEC, NOV, DEC, HEX},			/* 2 */
 {"door_call",	5, DEC, NOV, DEC, HEX, HEX, HEX, HEX},	/* 3 */
 {"door_return",4, DEC, NOV, HEX, DEC, DEC, DEC},	/* 4 */
};
#define	NDOORCODE	(sizeof(doortable)/sizeof(struct systable))


CONST	struct sysalias sysalias[] = {
	{ "exit",	SYS_exit	},
	{ "sbrk",	SYS_brk		},
	{ "getppid",	SYS_getpid	},
	{ "geteuid",	SYS_getuid	},
	{ "getpgrp",	SYS_pgrpsys	},
	{ "setpgrp",	SYS_pgrpsys	},
	{ "getsid",	SYS_pgrpsys	},
	{ "setsid",	SYS_pgrpsys	},
	{ "getpgid",	SYS_pgrpsys	},
	{ "setpgid",	SYS_pgrpsys	},
	{ "getegid",	SYS_getgid	},
	{ "sigset",	SYS_signal	},
	{ "sighold",	SYS_signal	},
	{ "sigrelse",	SYS_signal	},
	{ "sigignore",	SYS_signal	},
	{ "sigpause",	SYS_signal	},
	{ "msgctl",	SYS_msgsys	},
	{ "msgget",	SYS_msgsys	},
	{ "msgsnd",	SYS_msgsys	},
	{ "msgrcv",	SYS_msgsys	},
	{ "msgop",	SYS_msgsys	},
	{ "shmctl",	SYS_shmsys	},
	{ "shmget",	SYS_shmsys	},
	{ "shmat",	SYS_shmsys	},
	{ "shmdt",	SYS_shmsys	},
	{ "shmop",	SYS_shmsys	},
	{ "semctl",	SYS_semsys	},
	{ "semget",	SYS_semsys	},
	{ "semop",	SYS_semsys	},
	{ "uname",	SYS_utssys	},
	{ "ustat",	SYS_utssys	},
	{ "fusers",	SYS_utssys	},
	{ "exec",	SYS_execve	},
	{ "execl",	SYS_execve	},
	{ "execv",	SYS_execve	},
	{ "execle",	SYS_execve	},
	{ "execlp",	SYS_execve	},
	{ "execvp",	SYS_execve	},
	{ "sigfillset",	SYS_sigpending	},
	{ "getcontext",	SYS_context	},
	{ "setcontext",	SYS_context	},
	{ "hrtcntl",	SYS_hrtsys	},
	{ "hrtalarm",	SYS_hrtsys	},
	{ "hrtsleep",	SYS_hrtsys	},
	{ "hrtcancel",	SYS_hrtsys	},
#ifdef SYS_kaio
	{ "aioread",	SYS_kaio	},
	{ "aiowrite",	SYS_kaio	},
	{ "aiowait",	SYS_kaio	},
	{ "aiocancel",	SYS_kaio	},
	{ "aionotify",	SYS_kaio	},
#endif
	{ "audit",	SYS_auditsys	},
	{ "locking",	SYS_xenix	},
	{ "creatsem",	SYS_xenix	},
	{ "opensem",	SYS_xenix	},
	{ "sigsem",	SYS_xenix	},
	{ "waitsem",	SYS_xenix	},
	{ "nbwaitsem",	SYS_xenix	},
	{ "rdchk",	SYS_xenix	},
	{ "chsize",	SYS_xenix	},
	{ "ftime",	SYS_xenix	},
	{ "nap",	SYS_xenix	},
	{ "sdget",	SYS_xenix	},
	{ "sdfree",	SYS_xenix	},
	{ "sdenter",	SYS_xenix	},
	{ "sdleave",	SYS_xenix	},
	{ "sdgetv",	SYS_xenix	},
	{ "sdwaitv",	SYS_xenix	},
	{ "proctl",	SYS_xenix	},
	{ "door_create",	SYS_door	},
	{ "door_revoke",	SYS_door	},
	{ "door_info",		SYS_door	},
	{ "door_call",		SYS_door	},
	{ "door_return",	SYS_door	},
	{  NULL,	0	}	/* end-of-list */
};


/* return structure to interpret system call with sub-codes */
CONST struct systable *
subsys(syscall, subcode)
register int syscall;
register int subcode;
{
	register CONST struct systable *stp = NULL;

	if (subcode != -1) {
		switch (syscall) {
		case SYS_open:
			subcode = (subcode & O_CREAT)? 1 : 0;
			if ((unsigned)subcode < NOPENCODE)
				stp = &opentable[subcode];
			break;
		case SYS_signal:	/* signal() + sigset() family */
			switch(subcode & ~SIGNO_MASK) {
			default:	subcode = 0;	break;
			case SIGDEFER:	subcode = 1;	break;
			case SIGHOLD:	subcode = 2;	break;
			case SIGRELSE:	subcode = 3;	break;
			case SIGIGNORE:	subcode = 4;	break;
			case SIGPAUSE:	subcode = 5;	break;
			}
			if ((unsigned)subcode < NSIGCODE)
				stp = &sigtable[subcode];
			break;
		case SYS_msgsys:	/* msgsys() */
			if ((unsigned)subcode < NMSGCODE)
				stp = &msgtable[subcode];
			break;
		case SYS_semsys:	/* semsys() */
			if ((unsigned)subcode < NSEMCODE)
				stp = &semtable[subcode];
			break;
		case SYS_shmsys:	/* shmsys() */
			if ((unsigned)subcode < NSHMCODE)
				stp = &shmtable[subcode];
			break;
		case SYS_pgrpsys:	/* pgrpsys() */
			if ((unsigned)subcode < NPIDCODE)
				stp = &pidtable[subcode];
			break;
		case SYS_utssys:	/* utssys() */
			if ((unsigned)subcode < NUTSCODE)
				stp = &utstable[subcode];
			break;
		case SYS_sysfs:		/* sysfs() */
			if ((unsigned)subcode < NSFSCODE)
				stp = &sfstable[subcode];
			break;
		case SYS_sigpending:	/* sigpending()/sigfillset() */
			if ((unsigned)subcode < NSGPCODE)
				stp = &sgptable[subcode];
			break;
		case SYS_context:	/* [get|set]context() */
			if ((unsigned)subcode < NCTXCODE)
				stp = &ctxtable[subcode];
			break;
		case SYS_hrtsys:	/* hrtsys() */
			if ((unsigned)subcode < NHRTCODE)
				stp = &hrttable[subcode];
			break;
		case SYS_vtrace:	/* vtrace() */
			if ((unsigned)subcode < NVTRCODE)
				stp = &vtrtable[subcode];
			break;
#ifdef SYS_kaio
		case SYS_kaio:		/* kaio() */
			subcode &= ~AIO_POLL_BIT;
			if ((unsigned)subcode < NAIOCODE)
				stp = &aiotable[subcode];
			break;
#endif
		case SYS_xenix:		/* xenix() */
			subcode = (subcode >> 8) & 0xFF;	/* stupid */
			if ((unsigned)subcode < NXENCODE)
				stp = &xentable[subcode];
			break;
		case SYS_door:		/* doors */
			if ((unsigned)subcode < NDOORCODE)
				stp = &doortable[subcode];
		}
	} 

	if (stp == NULL)
		stp = &systable[((unsigned)syscall < SYSEND)? syscall : 0];

	return stp;
}

CONST char *
sysname(syscall, subcode)	/* return the name of the system call */
int syscall;
int subcode;
{
	register CONST struct systable * stp = subsys(syscall, subcode);
	register CONST char * name = stp->name;	/* may be NULL */

	if (name == NULL) {		/* manufacture a name */
		(void) sprintf(sys_name, "sys#%d", syscall);
		name = sys_name;
	}

	return name;
}

CONST char *
rawsigname(sig)		/* return the name of the signal */
int sig;		/* return NULL if unknown signal */
{
	/* belongs in some header file */
	extern int sig2str(int, char *);

	/*
	 * The C library function sig2str() omits the leading "SIG".
	 */
	(void) strcpy(raw_sig_name, "SIG");

	if (sig > 0 && sig2str(sig, raw_sig_name+3) == 0)
		return raw_sig_name;
	return NULL;
}

CONST char *
signame(sig)		/* return the name of the signal */
int sig;		/* manufacture a name for unknown signal */
{
	register CONST char * name = rawsigname(sig);

	if (name == NULL) {			/* manufacture a name */
		(void) sprintf(sig_name, "SIG#%d", sig);
		name = sig_name;
	}

	return name;
}

CONST char *
rawfltname(flt)		/* return the name of the fault */
int flt;		/* return NULL if unknown fault */
{
	register CONST char * name;

	switch (flt) {
	case FLTILL:	name = "FLTILL";	break;
	case FLTPRIV:	name = "FLTPRIV";	break;
	case FLTBPT:	name = "FLTBPT";	break;
	case FLTTRACE:	name = "FLTTRACE";	break;
	case FLTACCESS:	name = "FLTACCESS";	break;
	case FLTBOUNDS:	name = "FLTBOUNDS";	break;
	case FLTIOVF:	name = "FLTIOVF";	break;
	case FLTIZDIV:	name = "FLTIZDIV";	break;
	case FLTFPE:	name = "FLTFPE";	break;
	case FLTSTACK:	name = "FLTSTACK";	break;
	case FLTPAGE:	name = "FLTPAGE";	break;
	default:	name = NULL;		break;
	}

	return name;
}

CONST char *
fltname(flt)		/* return the name of the fault */
int flt;		/* manufacture a name if fault unknown */
{
	register CONST char * name = rawfltname(flt);

	if (name == NULL) {			/* manufacture a name */
		(void) sprintf(flt_name, "FLT#%d", flt);
		name = flt_name;
	}

	return name;
}
