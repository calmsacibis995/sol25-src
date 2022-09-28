/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)repquota.c	1.18	94/11/16 SMI"	/* SVr4.0 1.8	*/

/*
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * 		PROPRIETARY NOTICE (Combined)
 * 
 * This source code is unpublished proprietary information
 * constituting, or derived under license from AT&T's UNIX(r) System V.
 * In addition, portions of such source code were derived from Berkeley
 * 4.3 BSD under license from the Regents of the University of
 * California.
 * 
 * 
 * 
 * 		Copyright Notice 
 * 
 * Notice of copyright on this source code product does not indicate 
 * publication.
 * 
 * 	(c) 1986,1987,1988,1989  Sun Microsystems, Inc
 * 	(c) 1983,1984,1985,1986,1987,1988,1989  AT&T.
 * 	          All rights reserved.
 *  
 */

/*
 * Quota report
 */
#include <stdio.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mntent.h>

#define bcopy(f,t,n)    memcpy(t,f,n)
#define bzero(s,n)	memset(s,0,n)
#define bcmp(s,d,n)	memcmp(s,d,n)

#define index(s,r)	strchr(s,r)
#define rindex(s,r)	strrchr(s,r)

#include <sys/time.h>	
#include <sys/fs/ufs_quota.h>
#include <sys/stat.h>
#include <sys/mnttab.h>
#include <sys/vfstab.h>
#include <pwd.h>

#define LOGINNAMESIZE	8
struct username {
	struct username *u_next;
	u_short u_uid;
	char u_name[LOGINNAMESIZE + 1];
};
#define UHASH 997
struct username *uhead[UHASH];
struct username *lookup();
struct username *adduid();

char		*malloc();
char		*mntopt();

int	vflag;		/* verbose */
int	aflag;		/* all file systems */
char **listbuf;

extern int	optind;
extern char	*optarg;
char *hasvfsopt(), *hasmntopt();

#define QFNAME "quotas"
#define	CHUNK	50

#if DEV_BSIZE < 1024
#define dbtok(x)	((x) / (1024 / DEV_BSIZE))
#else
#define dbtok(x)	((x) * (DEV_BSIZE / 1024))
#endif

main(argc, argv)
	int argc;
	char **argv;
{
	struct mnttab mntp;
	struct vfstab vfsbuf;
	char **listp;
	int listcnt;
	int listmax = 0;
	char quotafile[MAXPATHLEN + 1];
	FILE *mtab, *vfstab;
	int errs = 0;
	int	status;
	int	opt;

	if ((listbuf = (char **)malloc(sizeof(char *) * CHUNK)) == NULL) {
		fprintf(stderr, "Can't alloc lisbuf array.");
		exit(31+1);
	}
	listmax = CHUNK;
	while ((opt = getopt (argc, argv, "avV")) != EOF) {
		switch (opt) {

		case 'v':
			vflag++;
			break;

		case 'a':
			aflag++;
			break;

		case 'V':		/* Print command line */
			{
				char		*opt_text;
				int		opt_count;

				(void) fprintf (stdout, "repquota -F UFS ");
				for (opt_count = 1; opt_count < argc ; opt_count++) {
					opt_text = argv[opt_count];
					if (opt_text)
						(void) fprintf (stdout, " %s ", opt_text);
				}
				(void) fprintf (stdout, "\n");
			}
			break;

		case '?':
			usage ();
		}
	}
	if (argc <= optind && !aflag) {
		usage ();
	}

	if (quotactl(Q_ALLSYNC, NULL, 0, NULL) < 0 && errno == EINVAL && vflag) {
		printf("Warning: Quotas are not compiled into this kernel\n");
	}
	/*
	 * If aflag go through vfstab and make a list of appropriate
	 * filesystems.
	 */
	if (aflag) {
		listp = listbuf;
		listcnt = 0;
		if ((vfstab = fopen(VFSTAB, "r")) == NULL) {
			fprintf(stderr, "Can't open ");
			perror(VFSTAB);
			exit(31+8);
		}
		while ((status = getvfsent(vfstab, &vfsbuf)) == NULL) {
			if (strcmp(vfsbuf.vfs_fstype, MNTTYPE_UFS) != 0 ||
			    (vfsbuf.vfs_mntopts == 0) ||
			    hasvfsopt(&vfsbuf, MNTOPT_RO) ||
			    (!hasvfsopt(&vfsbuf, MNTOPT_RQ) &&
			     !hasvfsopt(&vfsbuf, MNTOPT_QUOTA)))
				continue;
			*listp = malloc(strlen(vfsbuf.vfs_special) + 1);
			strcpy(*listp, vfsbuf.vfs_special);
			listp++;
			listcnt++;
			/* grow listbuf if needed */
			if (listcnt >= listmax) {
				listmax += CHUNK;
				listbuf = (char **) realloc(listbuf,
					sizeof(char *) * listmax);
				if (listbuf == NULL) {
					fprintf(stderr,
						"Can't grow listbuf.\n");
					exit(31+1);
				}
				listp = &listbuf[listcnt];
			}
		}
		(void) fclose(vfstab);
		*listp = (char *)0;
		listp = listbuf;
	} else {
		listp = &argv[optind];
		listcnt = argc - optind;
	}
	if ((mtab = fopen(MNTTAB, "r")) == NULL) {
		fprintf(stderr, "Can't open ");
		perror(MNTTAB);
		exit(31+8);
	}
	while ((status = getmntent(mtab, &mntp)) == NULL) {
		if (strcmp(mntp.mnt_fstype, MNTTYPE_UFS) == 0 &&
		    !hasmntopt(&mntp, MNTOPT_RO) &&
		    (oneof(mntp.mnt_special, listp, listcnt) ||
		     oneof(mntp.mnt_mountp, listp, listcnt)) ) {
			sprintf(quotafile, "%s/%s", mntp.mnt_mountp, QFNAME);
			errs +=
			    repquota(mntp.mnt_special,mntp.mnt_mountp,quotafile);
		}
	}
	(void) fclose(mtab);
	while (listcnt--) {
		if (*listp)
			fprintf(stderr, "Cannot report on %s\n", *listp);
	}
	if (errs > 0)
		exit(31+1);
	exit (0);
}

repquota(fsdev, fsfile, qffile)
	char *fsdev;
	char *fsfile;
	char *qffile;
{
	FILE *qf;
	u_short uid;
	struct dqblk dqbuf;
	struct stat statb;
	extern int errno;

	if (vflag || aflag)
		printf("%s (%s):\n", fsdev, fsfile);
	qf = fopen(qffile, "r");
	if (qf == NULL) {
		perror(qffile);
		return (1);
	}
	if (fstat(fileno(qf), &statb) < 0) {
		perror(qffile);
		fclose(qf);
		return (1);
	}
	header();
	for (uid = 0; ; uid++) {
		(void) fread(&dqbuf, sizeof(struct dqblk), 1, qf);
		if (feof(qf))
			break;
		if (!vflag &&
		    dqbuf.dqb_curfiles == 0 && dqbuf.dqb_curblocks == 0)
			continue;
		prquota(uid, &dqbuf);
	}
	fclose(qf);
	return (0);
}

header()
{

	printf(
"                      Block limits                      File limits\n"
	);
	printf(
"User           used   soft   hard    timeleft    used   soft   hard    timeleft\n"
	);
}

prquota(uid, dqp)
	u_short uid;
	struct dqblk *dqp;
{
	struct timeval tv;
	register struct username *up;
	char ftimeleft[80], btimeleft[80];

	if (dqp->dqb_bsoftlimit == 0 && dqp->dqb_bhardlimit == 0 &&
	    dqp->dqb_fsoftlimit == 0 && dqp->dqb_fhardlimit == 0)
		return;
	(void) time(&(tv.tv_sec));
	tv.tv_usec = 0;
	up = lookup(uid);
	if (up)
		printf("%-10s", up->u_name);
	else
		printf("#%-9d", uid);
	if (dqp->dqb_bsoftlimit && dqp->dqb_curblocks>=dqp->dqb_bsoftlimit) {
		if (dqp->dqb_btimelimit == 0) {
			strcpy(btimeleft, "NOT STARTED");
		} else if (dqp->dqb_btimelimit > tv.tv_sec) {
			fmttime(btimeleft,
			    (long)(dqp->dqb_btimelimit - tv.tv_sec));
		} else {
			strcpy(btimeleft, "EXPIRED");
		}
	} else {
		btimeleft[0] = '\0';
	}
	if (dqp->dqb_fsoftlimit && dqp->dqb_curfiles>=dqp->dqb_fsoftlimit) {
		if (dqp->dqb_ftimelimit == 0) {
			strcpy(ftimeleft, "NOT STARTED");
		} else if (dqp->dqb_ftimelimit > tv.tv_sec) {
			fmttime(ftimeleft,
			    (long)(dqp->dqb_ftimelimit - tv.tv_sec));
		} else {
			strcpy(ftimeleft, "EXPIRED");
		}
	} else {
		ftimeleft[0] = '\0';
	}
	printf("%c%c%7d%7d%7d%12s %7d%7d%7d%12s\n",
		dqp->dqb_bsoftlimit && 
		    dqp->dqb_curblocks >= 
		    dqp->dqb_bsoftlimit ? '+' : '-',
		dqp->dqb_fsoftlimit &&
		    dqp->dqb_curfiles >=
		    dqp->dqb_fsoftlimit ? '+' : '-',
		dbtok(dqp->dqb_curblocks),
		dbtok(dqp->dqb_bsoftlimit),
		dbtok(dqp->dqb_bhardlimit),
		btimeleft,
		dqp->dqb_curfiles,
		dqp->dqb_fsoftlimit,
		dqp->dqb_fhardlimit,
		ftimeleft
	);
}

fmttime(buf, time)
	char *buf;
	register long time;
{
	int i;
	static struct {
		int c_secs;		/* conversion units in secs */
		char * c_str;		/* unit string */
	} cunits [] = {
		{60*60*24*28, "months"},
		{60*60*24*7, "weeks"},
		{60*60*24, "days"},
		{60*60, "hours"},
		{60, "mins"},
		{1, "secs"}
	};

	if (time <= 0) {
		strcpy(buf, "EXPIRED");
		return;
	}
	for (i = 0; i < sizeof(cunits)/sizeof(cunits[0]); i++) {
		if (time >= cunits[i].c_secs)
			break;
	}
	sprintf(buf, "%.1f %s", (double)time/cunits[i].c_secs, cunits[i].c_str);
}

oneof(target, olistp, on)
	char *target;
	register char **olistp;
	register int on;
{
	char **listp = olistp;
	int n = on;

	while (n--) {
		if (*listp && strcmp(target, *listp) == 0) {
			*listp = (char *)0;
			return (1);
		}
		listp++;
	}
	return (0);
}

struct username *
lookup(uid)
	uid_t uid;
{
	register struct passwd *pwp;
	register struct username *up;

	for (up = uhead[uid % UHASH]; up != 0; up = up->u_next)
		if (up->u_uid == uid) {
			return (up);
		}
	if ((pwp = getpwuid((uid_t) uid)) == NULL) {
		return ((struct username *)0);
	}
	up = adduid(pwp->pw_uid);
	strncpy(up->u_name, pwp->pw_name, sizeof(up->u_name));
	return (up);
}

/*
 * adduid() should *ONLY* be called from lookup in order
 * to avoid duplicate entries.
 */
struct username *
adduid(uid)
	u_short uid;
{
	struct username *up, **uhp;

	up = (struct username *)calloc(1, sizeof(struct username));
	if (up == 0) {
		fprintf(stderr, "out of memory for username structures\n");
		exit(31+1);
	}
	uhp = &uhead[uid % UHASH];
	up->u_next = *uhp;
	*uhp = up;
	up->u_uid = uid;
	return (up);
}

usage ()
{
	fprintf(stderr, "ufs usage:\n");
	fprintf(stderr, "\trepquota [-v] -a \n");
	fprintf(stderr, "\trepquota [-v] filesys ...\n");
	exit(31+1);
}

#include <sys/errno.h>

quotactl (cmd, special, uid, addr)
	int		cmd;
	char		*special;
	int		uid;
	caddr_t		addr;
{
	int		fd;
	int		status;
	struct quotctl	quota;
	char		mountpoint[256];
	FILE		*fstab;
	struct mnttab	mntp;


	if ((special == NULL) && (cmd == Q_ALLSYNC)) {
	/*
	 * Find the mount point of the special device.   This is
	 * because the ioctl that implements the quotactl call has
	 * to go to a real file, and not to the block device.
	 */
		if ((fstab = fopen(MNTTAB, "r")) == NULL) {
			fprintf (stderr, "%s: ", MNTTAB);
			perror ("open");
			exit (31+1);
		}
		fd = -1;
		while ((status = getmntent(fstab, &mntp)) == NULL) {
			if (strcmp(mntp.mnt_fstype, MNTTYPE_UFS) != 0 ||
				hasmntopt(&mntp, MNTOPT_RO))
				continue;
			(void) strcpy(mountpoint, mntp.mnt_mountp);
			strcat (mountpoint, "/quotas");

			/* If we find *ANY* valid "quotas" file, use it */
			if ((fd = open(mountpoint, O_RDONLY)) >= 0)
				break;
		}
		fclose(fstab);
		if (fd == -1) {
			errno = ENOENT;
			printf("quotactl: no quotas file on any mounted file system\n");
			return (-1);
		}
	}
	quota.op = cmd;
	quota.uid = uid;
	quota.addr = addr;
	status = ioctl (fd, Q_QUOTACTL, &quota);
	close (fd);
	return (status);
}

char *
hasvfsopt(vfs, opt)
        register struct vfstab *vfs;
        register char *opt;
{
        char *f, *opts;
        static char *tmpopts;

        if (tmpopts == 0) {
                tmpopts = (char *)calloc(256, sizeof (char));
                if (tmpopts == 0)
                        return (0);
        }
        strcpy(tmpopts, vfs->vfs_mntopts);
        opts = tmpopts;
        f = mntopt(&opts);
        for (; *f; f = mntopt(&opts)) {
                if (strncmp(opt, f, strlen(opt)) == 0)
                        return (f - tmpopts + vfs->vfs_mntopts);
        }
        return (NULL);
}
