#include <bsd/bsd.h>
#include <bsd/string.h>
#include <bsd/stdlib.h>
#include <bsd/stdio.h>
#include <assert.h>
#include <openssl/ssl.h> 
#include <mbedtls/config.h>
#include <mbedtls/net.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/error.h>
#include <mbedtls/certs.h>
#include "module_ssl.h"
#include "../misc.h"
#include "../config.h"
#include "../net_utils.h"
#include "../contrib/slre.h"
#include "module_detector.h"

#define SRVREG_REFERRAL(rid,protoid,regex,forcessl,csensitive) {rid,protoid,regex,forcessl,csensitive},
#define MAX_SRVREGEX 2


static struct {
		const unsigned short	rid;
		const unsigned short 	protoid;
		const char		*regex;
		const bool		csensitive;
		const bool 		forcessl;
} services_regex[] {
  SRVREG_REFERRAL(0, 0,"^HTTP.*", false,true)
  SRVREG_REFERRAL(1, 0,".*SMTP.*", false,true)
  SRVREG_REFERRAL(2, 0,".*XMPP.*", false,true)
  SRVREG_REFERRAL(99, 0,".*[client_hello|server_hello].*", false,true) //SSL Special check
  SRVREG_REFERRAL(9999,9999,NULL,false,false)
};
	


bool detector::seclayer(char *host, int port) {
  try {
    ssl *l_ssl = new ssl();
    
    /*
    if(l_ssl->open(host,port)==false)
      return(false);
    */
    
    if(l_ssl->testport(host,port,0)==false) {
      delete(l_ssl);
      return(false);
    }
   
    delete(l_ssl);
    return(true);
  }
  catch(int iResult) {
    switch(iResult) {
      default:
	__logging("detector::seclayer: Unknow error\n");
	break;
    }
    
    return(false);
  }
}

int detector::process_data(void *data, int datalen) {
  int i = 0;
  char *regex = NULL;
  
  for(i=0;i<MAX_SRVREGEX;i++) {
    if((regex = (char *)services_regex[i].regex)==NULL) //runtime sanity check
      assert(regex==NULL);
    
    if(slre_match(regex,(const char *)data,datalen,NULL,0,services_regex[i].csensitive ? 1 : 0)>0) {
        regex=NULL;
	return(services_regex[i].protoid);
    }
  }
  regex=NULL;
  return(-1);
}


int detector::service_tcp(char *host, int port) {
  try {
    netutils *net = new netutils();
    int datalen = 4096;
    char *padding0 = (char *)"PADDING0\r\n\r\n\n\n";
    char *data = (char *)__allocate(datalen);
    
    if(net->tcp_open(host,port)==false) {
      __liberty(data);
      delete(net);
      throw 0;
    }
    
    if(net->tcp_recv(data,datalen)>1) {
	int proto=0;
	if((proto=detector::process_data(data,datalen))!=-1)
	  __liberty(data);
	  delete(net);
	  return(proto);      
    }
    
    if(net->tcp_send(padding0, strlen(padding0))>1) {
	if(net->tcp_recv(data, datalen)>1) {
	    int proto=0;
	    if((proto=detector::process_data(data,datalen))!=-1) {
	      __liberty(data);
	      delete(net);
	      return(proto);
	    }
	}
    }
    else {
	__liberty(data);
	delete(net);
	return(-1);
    }
    
    __liberty(data);
    delete(net);
    return(-1);
      
  }
  catch(int iResult) {
    switch(iResult) {
      case 0:
	__logging("detector::service_tcp: Connection error\n");
	break;
      default:
	__logging("detector::service_tcp: Unknow error\n");
	break;
    }
    
    return(-1);
  }
}
