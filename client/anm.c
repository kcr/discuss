/*
 *	$Source: /afs/dev.mit.edu/source/repository/athena/bin/discuss/client/anm.c,v $
 *	$Header: /afs/dev.mit.edu/source/repository/athena/bin/discuss/client/anm.c,v 1.2 1987-04-09 05:06:09 spook Exp $
 *
 *	$Log: not supported by cvs2svn $
 * Revision 1.1  87/04/08  23:41:52  rfrench
 * Initial revision
 * 
 */

#ifndef lint
static char *rcsid_anm_c = "$Header: /afs/dev.mit.edu/source/repository/athena/bin/discuss/client/anm.c,v 1.2 1987-04-09 05:06:09 spook Exp $";
#endif lint

#include "sys/file.h"
#include "ss.h"
#include "tfile.h"
#include "interface.h"
#include "config.h"
#include "rpc.h"
#include "globals.h"

extern tfile unix_tfile();

announce_mtg (argc, argv)
	int argc;
	char **argv;
{
	char subject[100],temp_file[100],*src_mtg,*dest_mtg;
	char *editor = NULL;
	int code,fd,public,i,txn_no;
	tfile tf;
	name_blk nbsrc, nbdest;

	if (argc < 3) {
		(void) fprintf(stderr, "Usage:  %s {-private | -public} mtg_to_announce mtg_to_announce_in\n", argv[0]);
		return;
	}

	public = 1;
	src_mtg = dest_mtg = NULL;

	i = 0;
	while (i++,--argc > 0) {
		if (*argv[i] == '-') {
			if (!strcmp(argv[i],"-public")) {
				public = 1;
				continue;
			}
			else if (!strcmp(argv[i],"-private")) {
				public = 0;
				continue;
			}
			else if (!strcmp(argv[i], "-editor") ||
				 !strcmp(argv[i], "-ed")) {
					 if (argc == 1) {
						 (void) fprintf(stderr, 
								"No argument to %s.\n", argv[i]);
						 return;
					 }
					 editor = argv[++i];
					 argc--;
					 continue;
				 }
			else if (!strcmp(argv[i], "-no_editor")) {
				editor = "";
				continue;
			}
			else {
				fprintf(stderr,
					"Unknown control argument %s\n",
					argv[i]);
				return;
			}
		}
		if (src_mtg)
			if (dest_mtg) {
				fprintf(stderr,
					"Extra arguments specified: %s\n",
					argv[i]);
				return;
			}
			else
				dest_mtg = argv[i];
		else
			src_mtg = argv[i];
	}

	if (!src_mtg || !dest_mtg) {
		fprintf (stderr,
			 "Two meeting names must be specified\n");
		return;
	}

	dsc_get_mtg (user_id, src_mtg, &nbsrc, &code);
	if (code != 0) {
		(void) fprintf (stderr,
				"%s: Meeting not found in search path.\n",
				src_mtg);
		return;
	}

	dsc_get_mtg (user_id, dest_mtg, &nbdest, &code);
	if (code != 0) {
		(void) fprintf (stderr,
				"%s: Meeting not found in search path.\n",
				dest_mtg);
		return;
	}

	printf("Please enter the body of the meeting announcement.\n");

	(void) sprintf(temp_file,"/tmp/mtg%d.%d",getuid(),getpid());
	(void) unlink(temp_file);

	if (edit(temp_file, editor)) {
		(void) fprintf(stderr,
		  "Error during edit; meeting not announced.\n");
		return;
	}

	fd = open(temp_file,O_RDONLY,0);
	if (fd < 0) {
		(void) fprintf(stderr,"Temporary file disappeared!\n");
		return;
	}

	tf = unix_tfile(fd);

	dsc_announce_mtg(&nbsrc, &nbdest, public, tf, &txn_no, &code);

	if (code) {
		(void) fprintf(stderr,"Error adding transation: %s\n",
			error_message(code));
		return;
	}

	(void) printf("Transaction [%04d] entered in the %s meeting.\n",
		      txn_no, nbdest.aliases[0]);

	(void) close(fd);

}