     /********************************************************************
      *                                                                  *
      *         Copyright (c) 1985 by                                    *
      *            Lewis Makepeace and Stan Zanarotti                    *
      *                                                                  *
      *                                                                  *
      *         All rights reserved.                                     *
      *                                                                  *
      ********************************************************************/

/*
 *
 * stubs.c -- These are stubs that handle the calling of routines.
 *
 */


/* Derived from CORE.PAS 06/21/86 by SRZ */

#include "../include/interface.h"
#include "../include/rpc.h"
#include "../include/tfile.h"
#include "../include/acl.h"

extern bool recvbool();
extern char *recvstr();
Acl *recv_acl();

/*
 *
 * add_trn () --
 * adds a transaction to the current meeting, either as a reply or an
 * original transaction.  Returns an error code, and the transaction number
 * given to the transaction
 *
 */
add_trn (mtg_name, source_file, subject, reply_trn, result_trn, result)
char *mtg_name;
tfile source_file;
char *subject;
trn_nums reply_trn;		/* trn replying to;  0 if original */
trn_nums *result_trn;		/* trn number given to added trn */
int *result;
{
     startsend(ADD_TRN);
     if (rpc_err) { *result = rpc_err; return; }
     sendstr(mtg_name);
     sendfile(source_file);
     sendstr(subject);
     sendint(reply_trn);
     sendit("discuss");
     senddata(source_file);
     if (rpc_err) { *result = rpc_err; return; }
     recvreply ();
     if (rpc_err) { *result = rpc_err; return; }
     *result_trn = recvint();
     *result = recvint();
     if (rpc_err) { *result = rpc_err; return; }
     return;
}

/*
 *
 * get_trn_info () --
 * returns information about the given transaction in info, with an error
 * code as its return argument
 *
 */
get_trn_info (mtg_name, trn, info, result)
char *mtg_name;
trn_nums trn;
trn_info *info;
int *result;
{
     startsend(GET_TRN_INFO);
     if (rpc_err) { *result = rpc_err; return; }
     sendstr(mtg_name);
     sendint(trn);
     sendit("discuss");
     if (rpc_err) { *result = rpc_err; return; }
     recvreply();
     if (rpc_err) { *result = rpc_err; return; }
     recv_trn_info(info);
     *result = recvint();
     if (rpc_err) { *result = rpc_err; return; }
     return;
}


/*
 *
 * delete_trn () --
 * deletes the given transaction from the current meeting.  Returns an
 * error code
 *
 */
delete_trn (mtg_name, trn, result)
char *mtg_name;
trn_nums trn;
int *result;
{
     startsend(DELETE_TRN);
     if (rpc_err) { *result = rpc_err; return; }
     sendstr(mtg_name);
     sendint(trn);
     sendit("discuss");
     if (rpc_err) { *result = rpc_err; return; }
     recvreply();
     if (rpc_err) { *result = rpc_err; return; }
     *result = recvint();
     if (rpc_err) { *result = rpc_err; return; }
     return;
}

/*
 *
 * retrieve_trn () --
 * retrieves a previously deleted transaction from the current meeting, if
 * possible.  trn must refer to a deleted transaction.  An error code is
 * returned
 *
 */
retrieve_trn (mtg_name, trn, result)
char *mtg_name;
trn_nums trn;
int *result;
{
     startsend(RETRIEVE_TRN);
     if (rpc_err) { *result = rpc_err; return; }
     sendstr (mtg_name);
     sendint (trn);
     sendit("discuss");
     if (rpc_err) { *result = rpc_err; return; }
     recvreply();
     if (rpc_err) { *result = rpc_err; return; }
     *result = recvint();
     if (rpc_err) { *result = rpc_err; return; }
     return;
}



/*
 *
 * create_mtg () --
 * Creates a new meeting with the given long_mtg name, where location is the
 * it's place in the hierarchy, and the long_mtg_name is its canonical name.
 * The chairman of the new meeting is the current user.
 *
 */
create_mtg (location, long_mtg_name, public, result)
char *location,*long_mtg_name;
bool public;
int *result;
{
     startsend(CREATE_MTG);
     if (rpc_err) { *result = rpc_err; return; }
     sendstr(location);
     sendstr(long_mtg_name);
     sendbool(public);
     sendit("discuss");
     if (rpc_err) { *result = rpc_err; return; }
     recvreply();
     if (rpc_err) { *result = rpc_err; return; }
     *result = recvint();
     if (rpc_err) { *result = rpc_err; return; }
     return;
}


/*
 *
 * old_get_mtg_info () --
 * returns information about the given meeting.  Return argument is an
 * error code
 *
 */
old_get_mtg_info (mtg_name, info, result)
char *mtg_name;
mtg_info *info;
int *result;
{
     startsend(OLD_GET_MTG_INFO);
     if (rpc_err) { *result = rpc_err; return; }
     sendstr(mtg_name);
     sendit("discuss");
     if (rpc_err) { *result = rpc_err; return; }
     recvreply();
     if (rpc_err) { *result = rpc_err; return; }
     old_recv_mtg_info(info);
     *result = recvint();
     if (rpc_err) { *result = rpc_err; return; }
     return;
}
/*
 *
 * get_mtg_info () --
 * returns information about the given meeting.  Return argument is an
 * error code
 *
 */
get_mtg_info (mtg_name, info, result)
char *mtg_name;
mtg_info *info;
int *result;
{
     startsend(GET_MTG_INFO);
     if (rpc_err) { *result = rpc_err; return; }
     sendstr(mtg_name);
     sendit("discuss");
     if (rpc_err) { *result = rpc_err; return; }
     recvreply();
     if (rpc_err) { *result = rpc_err; return; }
     recv_mtg_info(info);
     *result = recvint();
     if (rpc_err) { *result = rpc_err; return; }
     return;
}

/*
 *
 * get_trn () --
 * gets the given transaction, and feeds it through dest_file.  Returns an
 * error code
 *
 */
get_trn (mtg_name, trn, dest_file, result)
char *mtg_name;
trn_nums trn;
tfile dest_file;
int *result;
{
     startsend(GET_TRN);
     if (rpc_err) { *result = rpc_err; return; }
     sendstr(mtg_name);
     sendint(trn);
     sendfile(dest_file);
     sendit("discuss");
     if (rpc_err) { *result = rpc_err; return; }
     recvdata(dest_file);
     recvreply();
     if (rpc_err) { *result = rpc_err; return; }
     *result = recvint();
     if (rpc_err) { *result = rpc_err; return; }
     return;
}

/*
 *
 * remove_mtg () --
 * removes the given meeting from the meeting list.  This should
 * probably go ahead and physically remove the contents of the meeting
 * but that's not obvious if we have the access to do that.
 *
 */
remove_mtg (mtg_name, result)
char *mtg_name;
int *result;
{
     startsend(REMOVE_MTG);
     if (rpc_err) { *result = rpc_err; return; }
     sendstr(mtg_name);
     sendit("discuss");
     if (rpc_err) { *result = rpc_err; return; }
     recvreply();
     if (rpc_err) { *result = rpc_err; return; }
     *result = recvint();
     if (rpc_err) { *result = rpc_err; return; }
     return;
}
/*
 *
 * updated_mtg () -- Quick procedure to check if the meeting is updated
 *		     with respect to a given time and transaction number.
 *
 */
updated_mtg (mtg_name, date_attended, last, updated, result)
char *mtg_name;
int date_attended, last;
bool *updated;
int *result;
{
     startsend(UPDATED_MTG);
     if (rpc_err) { *result = rpc_err; return; }
     sendstr(mtg_name);
     sendint(date_attended);
     sendint(last);
     sendit("discuss");
     if (rpc_err) { *result = rpc_err; return; }
     recvreply();
     if (rpc_err) { *result = rpc_err; return; }
     *updated = recvbool();
     *result = recvint();
     if (rpc_err) { *result = rpc_err; return; }
     return;
}
#define rpccheck if (rpc_err) { *result = rpc_err; return; }
/*
 * get_acl () -- Get access control list.
 */
get_acl(mtg_name, result, list)
	char *mtg_name;
	int *result;		/* RETURN */
	Acl **list;		/* RETURN */
{

	startsend(GET_ACL);
	rpccheck;
	sendstr(mtg_name);
	sendit("discuss");
	rpccheck;
	recvreply();
	if (rpc_err) { *result = rpc_err; return; }
	*result = recvint();
	*list = recv_acl();
	rpccheck;
	return;
}

get_access(mtg_name, princ_name, modes, result)
	char *mtg_name;
	char *princ_name;
	char **modes;		/* RETURN */
	int *result;		/* RETURN */
{
	startsend(GET_ACCESS);
	rpccheck;
	sendstr(mtg_name);
	sendstr(princ_name);
	sendit("discuss");
	rpccheck;
	recvreply();
	if (rpc_err) { *result = rpc_err; return; }
	*modes = recvstr();
	*result = recvint();
	rpccheck;
	return;
}
set_access(mtg_name, princ_name, modes, result)
	char *mtg_name;
	char *princ_name;
	char *modes;
	int *result;		/* RETURN */
{
	startsend(SET_ACCESS);
	rpccheck;
	sendstr(mtg_name);
	sendstr(princ_name);
	sendstr(modes);
	sendit("discuss");
	rpccheck;
	recvreply();
	if (rpc_err) { *result = rpc_err; return; }
	*result = recvint();
	rpccheck;
	return;
}

delete_access(mtg_name, princ_name, result)
	char *mtg_name;
	char *princ_name;
	int *result;		/* RETURN */
{
	startsend(DELETE_ACCESS);
	rpccheck;
	sendstr(mtg_name);
	sendstr(princ_name);
	sendit("discuss");
	rpccheck;
	recvreply();
	if (rpc_err) { *result = rpc_err; return; }
	*result = recvint();
	rpccheck;
	return;
}

/*
whoami(ident)
	char **ident;
{
	int *result=0;
	startsend(WHO_AM_I);
	rpccheck;
	sendit("discuss");
	rpccheck;
	*ident = recvstr();
	rpccheck;
	return;
}
*/


/*
 *
 * recv_trn_info -- recv a trn_info struct.
 *
 */
recv_trn_info(tinfo)
trn_info *tinfo;
{
     tinfo -> version = recvint ();
     tinfo -> current = recvint ();
     tinfo -> prev = recvint ();
     tinfo -> next = recvint ();
     tinfo -> pref = recvint ();
     tinfo -> nref = recvint ();
     tinfo -> fref = recvint ();
     tinfo -> lref = recvint ();
     tinfo -> chain_index = recvint ();
     tinfo -> date_entered = recvint ();
     tinfo -> num_lines = recvint ();
     tinfo -> num_chars = recvint ();
     tinfo -> subject = recvstr ();
     tinfo -> author = recvstr ();
}
    
/*
 *
 * old_recv_mtg_info -- Recv a mtg info struct.
 *
 */
old_recv_mtg_info(minfo)
mtg_info *minfo;
{
     minfo -> version = recvint ();
     minfo -> location = recvstr ();
     minfo -> long_name = recvstr ();
     minfo -> chairman = recvstr ();
     minfo -> first = recvint ();
     minfo -> last = recvint ();
     minfo -> lowest = recvint ();
     minfo -> highest = recvint ();
     minfo -> date_created = recvint ();
     minfo -> date_modified = recvint ();
     minfo -> public_flag = recvbool ();
}

/*
 *
 * recv_mtg_info -- Recv a mtg info struct.
 *
 */
recv_mtg_info(minfo)
mtg_info *minfo;
{
     minfo -> version = recvint ();
     minfo -> location = recvstr ();
     minfo -> long_name = recvstr ();
     minfo -> chairman = recvstr ();
     minfo -> first = recvint ();
     minfo -> last = recvint ();
     minfo -> lowest = recvint ();
     minfo -> highest = recvint ();
     minfo -> date_created = recvint ();
     minfo -> date_modified = recvint ();
     minfo -> public_flag = recvbool ();
     minfo -> access_modes = recvstr ();
}

Acl *
recv_acl()
{
	/* The following code would lose points in 6.170... */
	register Acl *result = (Acl *) malloc(sizeof(Acl));
	register acl_entry *ae;
	register unsigned int len;
	len = recvint();
	result->acl_length = len;
	if (len > 1000) /* Bogus! */ {
		/*XXX Put some error checking in here */
	}
	result->acl_entries = (acl_entry *) malloc(sizeof(acl_entry) * len);
	for (ae = result->acl_entries; len; --len, ++ae) {
		ae->modes = recvstr();	
		ae->principal = recvstr();
	}
	return result;
}
		
