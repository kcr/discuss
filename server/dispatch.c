/*
 *
 * dispatch.c  -- Procedure to do the dispatching on an RPC call.
 *		  This contains the procedure table.
 *
 */

#include "../include/rpc.h"
#include "../include/interface.h"
#include "../include/tfile.h"
#include "../include/acl.h"

extern bool recvbool();
extern char *recvstr();
extern tfile recvfile();
extern char *malloc();
extern char rpc_caller [];
struct proc_table procs[] = {
     0, {0, 0, 0, 0, 0, 0, 0, 0},					/* unused */
     4, {STRTYPE, TFILETYPE, STRTYPE, INTTYPE,  0, 0, 0, 0},		/* add_trn */
     2, {STRTYPE, INTTYPE, 0, 0, 0, 0, 0, 0},				/* get_trn_info */
     2, {STRTYPE, INTTYPE, 0, 0, 0, 0, 0, 0},				/* delete_trn */
     2, {STRTYPE, INTTYPE, 0, 0, 0, 0, 0, 0},				/* retrieve_trn */
     3, {STRTYPE, STRTYPE, BOOLTYPE, 0, 0, 0, 0, 0},			/* create_mtg */
     1, {STRTYPE, 0, 0, 0, 0, 0, 0, 0},					/* old_get_mtg_info */
     0, { 0, 0, 0, 0, 0, 0, 0, 0},					/* start_mtg_info */
     0, { 0, 0, 0, 0, 0, 0, 0, 0},					/* next_mtg_info */
     3, {STRTYPE, INTTYPE, TFILETYPE, 0, 0, 0, 0, 0},			/* get_trn */
     1, {STRTYPE, 0, 0, 0, 0, 0, 0, 0},					/* remove_mtg */
     1, {STRTYPE, 0, 0, 0, 0, 0, 0, 0},					/* get_mtg_info */
     1, {STRTYPE, 0, 0, 0, 0, 0, 0, 0},					/* get_acl */
     2, {STRTYPE, STRTYPE, 0, 0, 0, 0, 0, 0}, 				/* get_access */
     3, {STRTYPE, STRTYPE, STRTYPE, 0, 0, 0, 0, 0},			/* set_access */
     2, {STRTYPE, STRTYPE, STRTYPE, 0, 0, 0, 0, 0},			/* delete_access */
     0, {0, 0, 0, 0, 0, 0, 0, 0}					/* whoami */
};

int	numprocs = sizeof (procs) / sizeof (procs [0]);

dispatch (procno)
int procno;
{
     char *c1,*c2,*c3;
     char *modes;
     int i1,i2;
     bool b1;
     tfile t1,mem_tfile();
     int result;
     dsc_acl *list;
     trn_info tinfo;
     mtg_info minfo;

     switch (procno) {

     /* add_trn (mtg_name, source_file, subject, reply_trn, result_trn, result) */
     case ADD_TRN:
 	  c1 = recvstr();			/* mtg_name */
	  t1 = recvfile();			/* source_file */
	  c2 = recvstr();			/* subject */
	  i1 = recvint();			/* reply_trn */
	  add_trn (c1, t1, c2, i1, &i2, &result);
	  startreply();
	  sendint(i2);
	  sendint(result);
	  sendreply();

	  tdestroy(t1);
	  free(c1);
	  free(c2);
	  break;

     /* get_trn_info (mtg_name, trn, info, result) */	  
     case GET_TRN_INFO:
	  c1 = recvstr();			/* mtg_name */
	  i1 = recvint();			/* trn */
	  get_trn_info (c1, i1, &tinfo, &result);
	  startreply();
	  send_trn_info(&tinfo);
	  sendint(result);
	  sendreply();

	  free(tinfo.subject);
	  free(tinfo.author);
	  free(c1);
	  break;

     /* delete_trn (mtg_name, trn, result) */
     case DELETE_TRN:
	  c1 = recvstr();			/* mtg_name */
	  i1 = recvint();			/* trn */
	  delete_trn (c1, i1, &result);
	  startreply();
	  sendint(result);
	  sendreply();
	  free(c1);
	  break;

     /* retrieve_trn (mtg_name, trn, result) */
     case RETRIEVE_TRN:
	  c1 = recvstr();			/* mtg_name */
	  i1 = recvint();			/* trn */
	  retrieve_trn (c1, i1, &result);
	  startreply();
	  sendint(result);
	  sendreply();
	  free(c1);
	  break;

     /* create_mtg (location, long_mtg_name, public, result) */
     case CREATE_MTG:
	  c1 = recvstr();			/* location */
	  c2 = recvstr();			/* long_mtg_name */
	  b1 = recvbool();			/* public flag */
	  create_mtg (c1, c2, b1, &result);
	  startreply();
	  sendint(result);
	  sendreply();
	  free(c1);
	  free(c2);
	  break;

     /* get_mtg_info (mtg_name, info, result) */
     case GET_MTG_INFO:
	  c1 = recvstr();			/* mtg_name */
	  get_mtg_info (c1, &minfo, &result);
	  startreply();
	  send_mtg_info(&minfo);
	  sendint(result);
	  sendreply();

	  free(minfo.location);
	  free(minfo.long_name);
	  free(minfo.chairman);
	  free(minfo.access_modes);
	  free(c1);
	  break;

     /* old_get_mtg_info (mtg_name, info, result) */
     case OLD_GET_MTG_INFO:
	  c1 = recvstr();			/* mtg_name */
	  get_mtg_info (c1, &minfo, &result);
	  startreply();
	  old_send_mtg_info(&minfo);
	  sendint(result);
	  sendreply();

	  free(minfo.location);
	  free(minfo.long_name);
	  free(minfo.chairman);
	  free(minfo.access_modes);
	  free(c1);
	  break;

     /* get_trn (mtg_name, trn, dest_file, result) */
     case GET_TRN:
	  c1 = recvstr();			/* mtg_name */
	  i1 = recvint();			/* trn */
	  t1 = recvfile ();			/* dest_file */
	  get_trn (c1, i1, t1, &result);
	  tdestroy (t1);
	  startreply();
	  sendint(result);
	  sendreply();
	  free(c1);
	  break;

     /* remove_mtg (mtg_name, result) */
     case REMOVE_MTG:
  	  c1 = recvstr();			/* mtg_name */
	  remove_mtg (c1, &result);
	  startreply();
	  sendint(result);
	  sendreply();
	  free(c1);
	  break;

     /* updated_mtg (mtg_name, date_attended, last, updated, result) */
     case UPDATED_MTG:
     	  c1 = recvstr();			/* mtg_name */
	  i1 = recvint();			/* date_attended */
	  i2 = recvint();			/* last */
	  updated_mtg (c1, i1, i2, &b1, &result);
	  startreply();
	  sendbool(b1);
	  sendint (result);
	  sendreply();
	  free(c1);
	  
	  break;
     /* get_acl (mtg_name) */
     case GET_ACL:
	  c1 = recvstr();			/* mtg_name */
	  get_acl (c1, &result, &list);
	  startreply();
	  sendint(result);
	  send_acl(list);
	  sendreply();
	  acl_destroy(list);
	  free(c1);
	  break;

     /* get_access(mtg_name, princ_name) */
     case GET_ACCESS:
          c1 = recvstr();
	  c2 = recvstr();
	  get_access(c1, c2, &modes, &result);
	  startreply();
	  sendstr(modes);
	  sendint(result);
	  sendreply();
	  free(modes);
	  free(c1);
	  free(c2);
	  
	  break;
     /* set_access(mtg_name, princ_name, mode) */
     case SET_ACCESS:
	  c1 = recvstr();	
	  c2 = recvstr();
	  c3 = recvstr();
	  set_access(c1, c2, c3, &result);
	  startreply();
	  sendint(result);
	  sendreply();
	  free(c1);
	  free(c2);
	  free(c3);
	  break;
     case DELETE_ACCESS:
          c1 = recvstr();
	  c2 = recvstr();
	  delete_access(c1, c2, &result);
	  startreply();
	  sendint(result);
	  sendreply();
	  free(c1);
	  free(c2);
	  break;
     case WHO_AM_I:
	  startreply();
	  sendstr(rpc_caller);
	  sendreply();
	  break;	  
     }
     return;					/* all done for now */
}

/*
 *
 * send_trn_info -- send a trn_info struct.
 *
 */
send_trn_info(tinfo)
trn_info *tinfo;
{
     sendint (tinfo -> version);
     sendint (tinfo -> current);
     sendint (tinfo -> prev);
     sendint (tinfo -> next);
     sendint (tinfo -> pref);
     sendint (tinfo -> nref);
     sendint (tinfo -> fref);
     sendint (tinfo -> lref);
     sendint (tinfo -> chain_index);
     sendint (tinfo -> date_entered);
     sendint (tinfo -> num_lines);
     sendint (tinfo -> num_chars);
     sendstr (tinfo -> subject);
     sendstr (tinfo -> author);
}
    
/*
 *
 * send_mtg_info -- Send a mtg info struct.
 *
 */
old_send_mtg_info(minfo)
mtg_info *minfo;
{
     sendint (minfo -> version);
     sendstr (minfo -> location);
     sendstr (minfo -> long_name);
     sendstr (minfo -> chairman);
     sendint (minfo -> first);
     sendint (minfo -> last);
     sendint (minfo -> lowest);
     sendint (minfo -> highest);
     sendint (minfo -> date_created);
     sendint (minfo -> date_modified);
     sendbool (minfo -> public_flag);
}
    
/*
 *
 * send_mtg_info -- Send a mtg info struct.
 *
 */
send_mtg_info(minfo)
mtg_info *minfo;
{
     sendint (minfo -> version);
     sendstr (minfo -> location);
     sendstr (minfo -> long_name);
     sendstr (minfo -> chairman);
     sendint (minfo -> first);
     sendint (minfo -> last);
     sendint (minfo -> lowest);
     sendint (minfo -> highest);
     sendint (minfo -> date_created);
     sendint (minfo -> date_modified);
     sendbool (minfo -> public_flag);
     sendstr (minfo -> access_modes);
}

/*
 *	send_acl -- Send an access control list.
 */
send_acl(acl)
	dsc_acl *acl;
{
	register dsc_acl_entry *ae;
	register int n;

	if (!acl) {
		sendint(0);
		return;
	}

	sendint (acl->acl_length);
	for (ae=acl->acl_entries, n=acl->acl_length;
	     n;
	     --n, ++ae) {
		sendstr(ae->modes);
		sendstr(ae->principal);
	}
}

