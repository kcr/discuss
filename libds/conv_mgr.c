/*
 *
 * conv_mgr () -- File that contains procedure to handle conversations.  This
 *		  allows multiplexing of RPC stream, setting up the correct
 *		  stream for the given module.
 *
 */

#include "rpc.h"

#define NULL 0

char *malloc ();


/* conversation structure */
struct conv {
     char *module;
     int port;
     char *hostname;
     char *service_id;
     rpc_conversation rc;
};

/* variables for managing conversation table */
static struct conv *conv_base;
static int num_convs;
static int max_convs;

static int cur_conv = -1;

/*
 *
 * get_module () -- Procedure to return the current module.  Returns a
 *		    character string (static storage)
 *
 */
char *
get_module ()
{
     if (conv_base == NULL || cur_conv == -1)
	  return (NULL);

     return (conv_base [cur_conv].module);
}

/*
 *
 * set_module () -- Procedure to bind the set of procedure calls with a
 *		    module.  Binds them for all remote procedure calls in
 *		    the current process.  Thus people who want to do this
 *		    temporarily should do a "get_module" to restore it back.
 *
 *
 */
set_module (module,fatal_error,result)
char *module;
int *fatal_error,*result;
{
     char *hostname, *service_id;
     int port,i;
     rpc_conversation rc;
     struct conv *convp;

     *fatal_error = 0;
     *result = 0;

     /* check to see if we've set up the module table */
     if (conv_base == NULL) {
	  conv_base = (struct conv *) malloc (5 * sizeof (struct conv));
	  max_convs = 5;
     }

     /* see if there's a real module there */
     if (!module) {
	     cur_conv = -1;
	     return;
     }

     /* check if we're setting our current module */
     if (cur_conv != -1) {
	  if (!strcmp (module, conv_base[cur_conv].module)) {
	       set_rpc (conv_base[cur_conv].rc);
	       return;
	  }
     }

     /* Now loop through all, looking for module names for comparison */
     for (i = 0; i < num_convs; i++) {
	  if (!strcmp (module, conv_base[i].module)) {	 /* match */
	       set_rpc (conv_base[i].rc);
	       cur_conv = i;
	       return;
	  }
     }

     /* not found -- we're going to have to resolve the module name. */
     resolve_module (module, &port, &hostname, &service_id, result);
     if (*result) {
	  *fatal_error = 1;
	  return;
     }

     /* Check through conversations, looking for resolved module */
     for (i = 0; i < num_convs; i++) {
	  convp = &conv_base [i];
	  if (convp -> port == port && !namcmp (hostname, convp -> hostname)
	      && !strcmp (service_id, convp -> service_id)) {  /* found match, record */
		   rc = convp -> rc;
		   set_rpc (rc);
		   goto create_entry;
	  }
     }

     /* Not found.  Create the rpc conversation */
     rc = open_rpc (hostname, port, service_id, result);
     if (rc == NULL) {
	  *fatal_error = 1;
	  return;				/* error code is set */
     }

create_entry:
     if (num_convs == max_convs) {
	  max_convs += 2;
	  conv_base = (struct conv *) realloc (conv_base, max_convs * sizeof (struct conv));
     }

     cur_conv = num_convs++;
     convp = &conv_base [cur_conv];
     convp -> rc = rc;
     convp -> hostname = malloc (strlen (hostname)+1);
     strcpy (convp -> hostname, hostname);
     convp -> service_id = malloc (strlen (service_id)+1);
     strcpy (convp -> service_id, service_id);
     convp -> module = malloc (strlen (module)+1);
     strcpy (convp -> module, module);
     convp -> port = port;
     
     return;
}

/*
 *
 * flush_convs () -- Routine to flush all conversations.
 *
 */
flush_convs ()
{
     int i,j;
     struct conv *convp;

     if (conv_base == NULL)
	  return;

     for (i = 0; i < num_convs; i++) {
	  convp = &conv_base[i];

	  if (convp -> rc)
	       close_rpc (convp -> rc);
	  free(convp -> module);
	  free(convp -> hostname);
	  free(convp -> service_id);
	
	  /* tromp on future conversations */
	  for (j = i+1; j < num_convs; j++) {
	       if (convp -> rc == conv_base[j].rc)
		    conv_base[j].rc = NULL;
	  }
	  convp -> rc = 0;
     }
     
     free (conv_base);
     conv_base = NULL;
     num_convs = 0;
     max_convs = 0;
     cur_conv = -1;
}