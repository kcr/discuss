/*
 *
 * List request for DISCUSS
 *
 * $Header: /afs/dev.mit.edu/source/repository/athena/bin/discuss/client/list.c,v 1.3 1986-09-10 18:57:27 wesommer Exp $
 * $Source: /afs/dev.mit.edu/source/repository/athena/bin/discuss/client/list.c,v $
 * $Locker:  $
 *
 * Copyright (C) 1986 by the MIT Student Information Processing Board
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  86/08/23  21:43:09  spook
 * moved timecheck for list into list module
 * 
 * Revision 1.1  86/08/22  00:23:56  spook
 * Initial revision
 * 
 *
 */

#include <stdio.h>
#include <strings.h>
#include "../include/ss.h"
#include "../include/tfile.h"
#include "../include/interface.h"
#include "../include/config.h"
#include "../include/dsc_et.h"
#include "globals.h"

char *ctime(), *malloc();
int	time_now, time_sixmonthsago, time_plusthreemonths;

list(sci_idx, argc, argv)
	int sci_idx;
	int argc;
	char **argv;
{
	int txn_no;
	trn_info t_info;
	char newtime[26];
	char *cp;
	int code;
	int idl;
	int i;
	int new_trn_no = -1;

	(void) time(&time_now); 
	time_sixmonthsago = time_now - 6*30*24*60*60; 
	time_plusthreemonths = time_now + 6*30*24*60*60;

	if (cur_mtg == (char *)NULL) {
		ss_perror(sci_idx, 0, "No current meeting.\n");
		return;
	}
	txn_no = m_info.first;

	get_trn_map(&code);
	if (code) {
		ss_perror(sci_idx, code, "Can't get list of transactions");
		return;
	}

	get_trn_info(cur_mtg, cur_trans, &t_info, &code);
	if (code != 0)
		t_info.current = -1;

	if (argc == 1) {
		for (i = 1; i <= m_info.last; i++)
			chosen_trn_map[i] = 1;
		new_trn_no = 42;
	}
	else
		for (i = 1; i <= m_info.last; i++)
			chosen_trn_map[i] = 0;

	while (argv++, argc-- > 1) {
		code = trn_select(&t_info, *argv);
		if (code != 0) {
			ss_perror(sci_idx, code, *argv);
			return;
		}
	}

	for (i = 1; i <= m_info.last; i++)
		if (chosen_trn_map[i]) {
			if (new_trn_no == -1) {
				new_trn_no = i;
				cur_trans = i;
			}
			get_trn_info(cur_mtg, i, &t_info, &code);
			if (code == DELETED_TRN) {
				goto next;
				/* should check -idl flag */
			}
			else if (code != 0) {
				ss_perror(sci_idx, code,
					  "Can't read trn info");
				break;
			}
			cp=ctime(&t_info.date_entered);
			if((t_info.date_entered < time_sixmonthsago) ||
			   (t_info.date_entered > time_plusthreemonths))
				(void) sprintf(newtime, "%-7.7s %-4.4s",
					       cp+4, cp+20);
			else
				(void) sprintf(newtime, "%-12.12s", cp+4);
			/*
			 * If author ends with current realm, punt the realm.
			 */
			if ((cp=index(t_info.author, '@')) != NULL)
				if (!strcmp(cp+1, REALM))
					*cp = '\0';

			if (strlen(t_info.author) > 15) {
				(void) strcpy(&t_info.author[12], "...");
			}
			(void) sprintf(buffer, "(%d)", t_info.num_lines);
			if (strlen(t_info.subject) > 36) {
				(void) strcpy(&t_info.subject[33], "...");
			}
			(void) printf(" [%04d]%c%5s %s %-15s %-20s\n",
				      t_info.current,
				      (t_info.current == cur_trans)?'*':' ',
				      buffer,
				      newtime,
				      t_info.author,
				      t_info.subject);
		next:
			(void) free (t_info.author);
			(void) free (t_info.subject);
		}
	(void) free((char *)chosen_trn_map);
	if (new_trn_no == -1)
		(void) fprintf(stderr, "list: No transactions selected\n");
}
