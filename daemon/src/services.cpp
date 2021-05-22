#include <bsd/bsd.h>
#include <bsd/stdio.h>
#include <bsd/string.h>
#include <bsd/stdlib.h>
#include <openssl/ssl.h> 
#include <mbedtls/config.h>
#include <mbedtls/net.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/error.h>
#include <mbedtls/certs.h>
#include "config.h"
#include "output.h"
#include "misc.h"
#include "modules/protocol_http.h"
#include "modules/module_ssl.h"
#include "modules/module_https.h"
#include "services.h"

#define SERVICE_REFERRAL(id,s,ptr_func) { id,s,ptr_func }


static struct st_service srv[MAX_SERVICES] = { 
  SERVICE_REFERRAL(0, "HTTP", &services::chk_http),
  SERVICE_REFERRAL(1, "HTTPS", &services::chk_https),
  /*{2, "FTP", &services::chk_https},
  {3, "TELNET", &services::chk_https},*/
  SERVICE_REFERRAL(4, "SSH", &services::chk_https),
  //{5, "SMTP", &services::chk_https},
  {0, NULL, NULL},
};



void services::chk_http(const char *ip, st_output_node *node) {
    http *lhttp = new http();
  
    try {
      __logging("OK HTTP OPEN PORT %s:%d\n", ip, node->port);
    
      lhttp->request_init(0,"/",1,ip,node->port);
      lhttp->request_make();
      lhttp->request_run();
      lhttp->request_headextract();
    
	if(lhttp->reply)
	  strlcpy(node->finger,(const char *)lhttp->reply,MAX_OUTPUT_FINGER);
	else
	  throw 0;
      
	strlcpy(node->cert,(const char *)"ANY", 4);
	strlcpy(node->service,(const char *)services::getServiceByID(0),5);
	node->status=1;
    }
    catch(int iResult) {
      switch(iResult) {
	case 0:
	  __logging("services::chk_http: Invalid finger %s:%d\n", ip, node->port);
	  break;
	default:
	  __logging("services::chk_http: Unknow error %x", iResult);
	  break;
      }
      
    }
					      
}
void services::chk_https(const char *ip, st_output_node *node) {
    https *lhttps = new https();
  
    try {
      __logging("OK HTTPS OPEN PORT %s:%d\n", ip, node->port);
    
      lhttps->request_init(0,"/",1,ip,node->port);
      lhttps->request_make();
      lhttps->request_run();
      lhttps->request_headextract();
    
	if(lhttps->reply)
	  strlcpy(node->finger,(const char *)lhttps->reply,MAX_OUTPUT_FINGER);
	else
	  throw 0;
      
	strlcpy(node->cert,(const char *)"ANY", 4);
	strlcpy(node->service,(const char *)services::getServiceByID(1),5);
	node->status=1;
    }
    catch(int iResult) {
      switch(iResult) {
	case 0:
	  __logging("services::chk_https: Invalid finger %s:%d\n", ip, node->port);
	  break;
	default:
	  __logging("services::chk_https: Unknow error %x", iResult);
	  break;
      }
      
    }
}


bool services::srvcheck(const char *ip,st_output_node *node) {
  try {
    int serviceid = config::get_servicebyport(node->port);
    
    srv[serviceid].chk_func(ip,node);
    
    if(node->status==1)
      return(true);
    
    return(false);
    
    
  }
  catch(int iResult) {
    __logging("services::srvcheck: Unknow Error");
    return(false);
  }
}


const char *services::getServiceByID(int id) {
    if(id>MAX_SERVICES)
      return(NULL);
    
    return(srv[id].name);
}
