/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*LINTLIBRARY*/
#ident	"@(#)getvol.c	1.4	93/03/24 SMI"       /* SVr4.0 1.5.2.1 */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <devmgmt.h>

extern char	*optarg;
extern int	optind,
		ckquit,
		ckwidth;

extern char	*get_prog_name(void);
extern char	*set_prog_name(char *name);
char	*label, *fsname;
char	*prompt;
int	options = 0;
int	kpid = (-2);
int	signo = SIGKILL;

main(argc, argv)
int argc;
char *argv[];
{
	char newlabel[128];
	int c, n;

	(void) set_prog_name(argv[0]);

	while((c=getopt(argc, argv, "fFownx:l:p:k:s:?QW:")) != EOF) {
		switch(c) {
		  case 'Q':
			ckquit = 0;
			break;

		  case 'W':
			ckwidth = atol(optarg);
			break;

		  case 'f':
			options |= DM_FORMAT;
			break;

		  case 'F':
			options |= DM_FORMFS;
			break;

		  case 'o':
			options |= DM_OLABEL;
			break;

		  case 'n':
			options |= DM_BATCH;
			break;

		  case 'w':
			options |= DM_WLABEL;
			break;

		  case 'l':
			if(label)
				usage();
			label = strcpy(newlabel, optarg);
			break;

		  case 'p':
			prompt = optarg;
			break;

		  case 'x':
			if(label)
				usage();
			label = optarg;
			options |= DM_ELABEL;
			break;

		  case 'k':
			kpid = atol(optarg);
			break;
			
		  case 's':
			signo = atol(optarg);
			break;

		  default:
			usage();
		}
	}

	if((optind+1) != argc)
		usage();

	switch(n = getvol(argv[optind], label, options, prompt)) {
	  case 0:
		break;

	  case 1:
		progerr("unable to access device <%s>\n", argv[optind]);
		exit(1);

	  case 2:
		progerr("unknown device <%s>\n", argv[optind]);
		exit(2);

	  case 3:
		if(kpid > -2)
			(void) kill(kpid, signo);
		exit(3);

	  case 4:
		progerr("bad label on <%s>", argv[optind]);
		break;

	  default:
		progerr("unknown device error");
	}

	exit(n);
}

usage()
{
	fprintf(stderr, "usage: %s [-owfF] [-x extlabel] [-l [fsname],volname] device\n", get_prog_name());
	fprintf(stderr, "usage: %s [-n] [-x extlabel] [-l [fsname],volname] device\n", get_prog_name());
	exit(1);
}
