/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)mount.c	1.11	94/08/30 SMI"	/* SVr4.0 1.1	*/

#include	<stdio.h>
#include	<signal.h>
#include	<string.h>
#include	<unistd.h>
#include	<errno.h>
#include	<sys/mnttab.h>
#include	<sys/mount.h>
#include	<sys/types.h>
#include	<locale.h>
#include	<fslib.h>

#define	NAME_MAX	64	/* sizeof "fstype myname" */

#define	MNTOPT_DEV	"dev"

#define	FSTYPE		"fd"

#define	RO_BIT		1

#define	READONLY	0
#define	READWRITE	1
#define	IGNORE		2

extern int	errno;
extern int	optind;
extern char	*optarg;

extern time_t	time();

static int	ignore_flag;
static int	roflag = 0;
static int	mflg = 0;

static char	typename[NAME_MAX], *myname;
static char	fstype[] = FSTYPE;
static char	*myopts[] = {
	"ro",
	"rw",
	"ignore",
	NULL
};


static void usage(void);
static void do_mount(char *, char *, int);


main(argc, argv)
	int	argc;
	char	**argv;
{
	char	*special, *mountp;
	char	*options, *value;
	int	errflag = 0;
	int	cc, rwflag = 0;
	struct mnttab	mm;
	char	optbuf[256];


	(void) setlocale(LC_ALL, "");

#if !defined(TEXT_DOMAIN)
#define	TEXT_DOMAIN "SYS_TEST"
#endif
	(void) textdomain(TEXT_DOMAIN);

	myname = strrchr(argv[0], '/');
	if (myname)
		myname++;
	else
		myname = argv[0];
	(void) sprintf(typename, "%s %s", fstype, myname);
	argv[0] = typename;

	/*
	 *	check for proper arguments
	 */

	while ((cc = getopt(argc, argv, "?o:rm")) != -1)
		switch (cc) {
		case 'r':
			if (roflag & RO_BIT)
				errflag = 1;
			else
				roflag |= RO_BIT;
			break;
		case 'm':
			mflg++;
			break;
		case 'o':
			options = optarg;
			while (*options != '\0')
				switch (getsubopt(&options, myopts, &value)) {
				case READONLY:
					if ((roflag & RO_BIT) || rwflag)
						errflag = 1;
					else
						roflag |= RO_BIT;
					break;
				case READWRITE:
					if ((roflag & RO_BIT) || rwflag)
						errflag = 1;
					else
						rwflag = 1;
					break;
				case IGNORE:
					ignore_flag = 1;
					break;
				default:
					fprintf(stderr,
				gettext("%s: illegal -o suboption -- %s\n"),
						typename, value);
					errflag++;
				}
			break;
		case '?':
			errflag = 1;
			break;
		}

	/*
	 *	There must be at least 2 more arguments, the
	 *	special file and the directory.
	 */

	if (((argc - optind) != 2) || (errflag))
		usage();

	special = argv[optind++];
	mountp = argv[optind++];

	if (geteuid() != 0) {
		fprintf(stderr, gettext("%s: not super user\n"), myname);
		exit(2);
	}

	(void) signal(SIGHUP,  SIG_IGN);
	(void) signal(SIGQUIT, SIG_IGN);
	(void) signal(SIGINT,  SIG_IGN);
	/*
	 *	Perform the mount.
	 *	Only the low-order bit of "roflag" is used by the system
	 *	calls (to denote read-only or read-write).
	 */
	do_mount(special, mountp, roflag & RO_BIT);

	if (!mflg) {
		mm.mnt_special = special;
		mm.mnt_mountp = mountp;
		mm.mnt_fstype = fstype;
		mm.mnt_mntopts = optbuf;
		cc = sprintf(optbuf, "%s", (roflag & RO_BIT) ? "ro" : "rw");
		if (ignore_flag)
			(void) sprintf(&optbuf[ cc ], ",ignore");
		if (fsaddtomtab(&mm))
			exit(1);
	}
	exit(0);
	/* NOTREACHED */
}


static void
rpterr(bs, mp)
	register char *bs, *mp;
{
	switch (errno) {
	case EPERM:
		fprintf(stderr,
			gettext("%s: not super user\n"), myname);
		break;
	case ENXIO:
		fprintf(stderr,
			gettext("%s: %s no such device\n"), myname, bs);
		break;
	case ENOTDIR:
		fprintf(stderr,
gettext("%s: %s not a directory\n\tor a component of %s is not a directory\n"),
			myname, mp, bs);
		break;
	case ENOENT:
		fprintf(stderr,
			gettext("%s: %s or %s, no such file or directory\n"),
			myname, bs, mp);
		break;
	case EINVAL:
		fprintf(stderr, gettext("%s: %s is not this fstype.\n"),
			myname, bs);
		break;
	case EBUSY:
		fprintf(stderr,
			gettext("%s: %s is already mounted, %s is busy,\n\tor allowable number of mount points exceeded\n"),
			myname, bs, mp);
		break;
	case ENOTBLK:
		fprintf(stderr, gettext("%s: %s not a block device\n"),
			myname, bs);
		break;
	case EROFS:
		fprintf(stderr,
			gettext("%s: %s write-protected\n"), myname, bs);
		break;
	case ENOSPC:
		fprintf(stderr,
			gettext("%s: the state of %s is not okay\n\tand it was attempted to mount read/write\n"),
			myname, bs);
		break;
	default:
		perror(myname);
		fprintf(stderr, gettext("%s: cannot mount %s\n"), myname, bs);
	}
}


static void
do_mount(char *special, char *mountp, int rflag)
{
	if (mount(special, mountp, rflag | MS_DATA, fstype, (char *)NULL, 0)) {
		rpterr(special, mountp);
		exit(2);
	}
}


static void
usage()
{
	fprintf(stderr,
		gettext("%s usage:\n%s [-F %s] [-r] [-o specific_options] {special | mount_point}\n%s [-F %s] [-r] [-o specific_options] special mount_point\n"),
		fstype, myname, fstype, myname, fstype);
	exit(1);
}
