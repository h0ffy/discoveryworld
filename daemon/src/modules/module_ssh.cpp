#include <bsd/bsd.h>
#include <bsd/string.h>
#include <bsd/stdlib.h>
#include <bsd/stdio.h>
#include <libssh/libssh.h> 
#include <libssh/legacy.h>
#include "../net_utils.h"
#include "../misc.h"
#include "module_ssh.h"


#define CR "\n\r"

/*
// FIX
NOTE: 	The used functions to retreive ssh hash is deprecated
	and today only save hash of pubkey retreive more information
	with next functions and solve deprecated.

LIBSSH_API char *ssh_get_issue_banner(ssh_session session);
LIBSSH_API int ssh_get_openssh_version(ssh_session session);

LIBSSH_API int ssh_get_publickey(ssh_session session, ssh_key *key);

enum ssh_publickey_hash_type {
    SSH_PUBLICKEY_HASH_SHA1,
    SSH_PUBLICKEY_HASH_MD5
};
LIBSSH_API int ssh_get_publickey_hash(const ssh_key key,
                                      enum ssh_publickey_hash_type type,
                                      unsigned char **hash,
                                      size_t *hlen);
*/



ssh::ssh()  {
    this->cert  = (char *)__allocate(MAX_SSH_SIZE);
    this->reply = (char *)__allocate(MAX_SSH_SIZE);
  /*  bzero(this->cert, MAX_SSH_SIZE);
    bzero(this->reply, MAX_SSH_SIZE);
  */
}
ssh::~ssh() {
    __liberty(this->cert);
    __liberty(this->reply);
}


int ssh::finger(const char *host, int port) {
	ssh_session session;
	int res, hlen;
	unsigned char *hash = NULL; 
	char *hexa;
	//char *banner;
	
    try {
// SSH GET CERT AND BANNER
	  while(ssh_init()!=0)
	    __logging("ssh::finger: SSH initialization error\n");
	  
	  while((session = ssh_new())==NULL)
	    __logging("ssh::finger: SSH construct error\n");
	
      
      
	  if(ssh_options_set(session, SSH_OPTIONS_HOST, host)<0) throw 1;
	  if(ssh_options_set(session, SSH_OPTIONS_PORT, (void *)&port)<0) throw 2;
	  if((res=ssh_connect(session))!=SSH_OK) {
		  throw 3;
	  }
	  else {
		  if(ssh_get_serverbanner(session)!=NULL)
		      strlcpy(this->reply,ssh_get_serverbanner(session),MAX_SSH_SIZE);
		  else
		      this->reply = NULL;
		  
		  hlen = ssh_get_pubkey_hash(session, &hash); //note: function deprecated change to ssh_get_publickey(session,&key); 
							      //ssh_get_publickey_hash(&key,0,&hash,&hash_len);
		  if((hexa = (char *)ssh_get_hexa(hash,hlen))==NULL)	throw 5;

		  //this->cert = (char *)__allocate(strlen((const char*)hexa)*2);
		  bzero(this->cert,MAX_SSH_SIZE);
		  strlcpy(this->cert,(const char *)hexa, MAX_SSH_SIZE);
		  //strlcat(this->cert,(const char *)CR,MAX_SSH_SIZE);
		  //strlcat(this->cert,(const char *)ssh_get_issue_banner(session),MAX_SSH_SIZE);
		  //strlcat(this->cert,(const char *)CR,MAX_SSH_SIZE);
		  
		  ssh_clean_pubkey_hash(&hash);
		  ssh_disconnect(session);
		  ssh_free(session);
		  ssh_finalize();	
	  }

  // SSH MANUAL FINGER
      if(this->reply==NULL) {
	  netutils *net = new netutils();

	  if(net->tcp_open(host,port)==1) {
	      if(net->tcp_recv(this->reply,2000)<1) {
		sprintf(this->reply, "Reply Error");
		throw 4;
	    }
	    net->tcp_close();
	  }
      }
    }
    catch(int iResult) {
      switch(iResult) {
	case 1:
	  __logging("ssh::finger: Error on set host option %s,%d", host, port);
	  return(-1); 
	case 2:
	  __logging("ssh::finger: Error on set port option %s,%d", host, port);
	  return(-1); 
	case 3:
	  __logging("ssh::finger: Error connection error %s,%d", host, port);
	  return(-1); 
	case 4:
	  __logging("ssh::finger: Warning no adquire finger %s,%d", host, port);
	  return(0); 
	case 5:
	  __logging("ssh::finger: Error ssh_get_hexa %s,%d", host, port);
	  return(-1); 
	default:
	  __logging("ssh::finger: Uknnow Error %s,%d", host, port);
	  return(-1); 
      }
    }
      

	return 0;
}
