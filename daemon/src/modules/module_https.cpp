#include <bsd/bsd.h>
#include <bsd/string.h>
#include <bsd/stdlib.h>
#include <bsd/stdio.h>
#include <openssl/ssl.h> 
#include <mbedtls/config.h>
#include <mbedtls/net.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/error.h>
#include <mbedtls/certs.h>
#include <mbedtls/compat-1.3.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../config.h"
#include "../misc.h"
#include "../net_utils.h"
#include "protocol_http.h"
#include "module_ssl.h"
#include "module_https.h"



https::https() {
    lhttp = new http();
    sl = new ssl();    
}

https::~https() {
    delete lhttp;
    delete sl;
}

/*
bool https::request_headextract() {
    return(this->lhttp->request_headextract());
}
*/
void https::request_make() {
    return(lhttp->request_make());
}

bool https::request_run() {
    int recvlen = 0;
    unsigned char *data;

    try {
      
      if(sl->send((const unsigned char *)lhttp->request,strlen(lhttp->request))<0)
	  throw 0;
      
      if((recvlen=sl->recv())<0)
	  throw 1;
 
      if((this->request = this->lhttp->request)==NULL)
	  return(false);
      
      if((this->reply = this->sl->reply)==NULL)
	  return(false);
     
      if((this->reply_count = strlen((const char *)this->reply))>0)
	return(false);

      
      return(true);
      
    }
    catch(int iResult) {
     switch(iResult) {
	case 0:
	  __logging("https::request_run: Error on send data %d bytes", strlen(lhttp->request));
	  break;
	case 1:
	  __logging("https::request_run: Error on recv data %d bytes", recvlen);
	  break;
	default:
	  __logging("https::request_run: Unknow error");
	  break;
     }
     
      
      return(false);
    }
    
  
}

bool https::request_add_header(const char *header, const char *value) {
    try {
      return(lhttp->request_add_header(header,value));
    }
    catch(int iResult) {
	switch(iResult) {
	  default:
	    __logging("https::request_add_header: Unknow Error %s:%s", header, value);
	}
	
	return(false);
    }  
}

bool https::request_add_data(char *data) {
    try {
      return(lhttp->request_add_data(data));
    }
    catch(int iResult) {
	switch(iResult) {
	  default:
	    __logging("https::request_add_data: Unknow Error %s", data);
	}
	
	return(false);
    }
 
}

bool https::request_init(int method,const char *path, int version,const char *host,int port) {
    unsigned char *new_ip = (unsigned char *)__allocate(128);
    
    try {
      
      lhttp->request_init(method,path,version,host,port);
      lhttp->request_make();
      
      if(netutils::resolve(host,(char *)new_ip)==1) {
	 if(sl->open((const char *)new_ip,port)==false)
	    throw 0;
      }
      else {
	if(sl->open((char *)host,port)==false)
	    throw 0;
      }
      //lhttp->request_headextract();
      __liberty(new_ip);
      return(true);
    }
    catch(int iResult) {
	switch(iResult) {
	  case 0:
	    __logging("https::request_init: Invalid connection %s:%d\n", host, port);
	    break;
	  default:
	    __logging("https::request_init: Unknow Error %s:%d\n", host, port);
	    break;
	}
	
	if(new_ip)
	  __liberty(new_ip);
	
	return(false);
    }
  
}

bool https::finger(const char *ip, int port) {
  bool ret = this->request_init(0,"/",1,ip,port);
  this->request_make();
  this->request_run();
  this->request_headextract(); // @note: change to this->lhttp->request_headextract();
  
  if(strlen((const char *)this->reply)==0)
    ret = false;
  
  return(ret);
}
/*
 * @note: duplicated function in HTTP use this function watch this case
*/
bool https::request_headextract() {
  int b=0,i=0;
  
  for(i=0;i<this->reply_count;i++) {
	  if(this->reply[i] == '\n' && this->reply[i+1] == '\n') {  this->reply[i]='\0'; }
							  
	  else if(this->reply[i] == '\r' && this->reply[i+1] == '\n' &&
		  this->reply[i+2] == '\r' && this->reply[i+3] == '\n'  ) {
			  this->reply[i]='\0';
			  b++;
		  }
															  
	  else if(this->reply[i] == '\n' && this->reply[i+1] == '\r' &&
		  this->reply[i+2] == '\n' && this->reply[i+3] == '\r'  ) {
		      this->reply[i]='\0';
		      b++;
	      }
  }


  return(!b ? true : false);
 
}
