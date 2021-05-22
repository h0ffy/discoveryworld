#include <bsd/bsd.h>
#include <bsd/string.h>
#include <bsd/stdlib.h>
#include <bsd/stdio.h>
#include <mbedtls/entropy.h>
#include <mbedtls/entropy_poll.h>
#include <openssl/ssl.h> 
#include <mbedtls/config.h>
#include <mbedtls/net.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/error.h>
#include <mbedtls/certs.h>
#include <mbedtls/compat-1.3.h>
#include "../net_utils.h"
#include "../config.h"
#include "../misc.h"
#include "module_ssl.h"




//hardcoded client hello for manual test
const unsigned char client_hello[] = 	"\x16\x03\x01" 			//__SSL HEAD
					"\x00\x5f"			// 
					"\x01"				//!client_hello
					"\x00\x00\x5b"			//len
					"\x03\x01"			//version
					"JennyLab"
					 "isSexy"
					"\x00"				//session id len
					"\x00\x2c"			//ciphers
					"\x00\x39\x00\x38\x00\x35\x00\x16\x00\x13\x00\x0a"
					"\x00\x33\x00\x32\x00\x9a\x00\x99\x00\x2f\x00\x96"
					"\x00\x05\x00\x04\x00\x15\x00\x12\x00\x09\x00\x14"
					"\x00\x11\x00\x08\x00\x06\x00\x03"
					"\x02\x01\x00"			//compressor
					"\x00\x05\xff\x01\x00\x01\x00";	//renegotiation
  

ssl::ssl() {
   try {
     char *pers = (char *)"JennySSL";
     bzero(&this->ctx, sizeof(ssl_context));
     bzero(&this->cacert, sizeof(mbedtls_x509_crt));
     mbedtls_entropy_init(&this->entropy);
     
     
     this->request = (unsigned char *)malloc(MAX_SSL_BUFF);
     this->reply = (unsigned char *)malloc(MAX_SSL_BUFF);
     bzero(this->request, MAX_SSL_BUFF);
     bzero(this->reply, MAX_SSL_BUFF);
     
     mbedtls_ctr_drbg_init(&this->ctr_ctx); 
     //,entropy_func,
     mbedtls_entropy_init(&this->entropy); 
		 //,(const unsigned char *)pers, strlen(pers)!=
	   
}
   catch(int iResult) {
      switch(iResult) {
	case 0:
	  __logging("ssl::ssl: Init crt instance\n");
	  break;  
	case 1:
	  __logging("ssl::ssl: Init entropy\n");
	  break;
	default:
	  __logging("ssl::ssl: Unknow error\n");
	  break;
	
      }
   }

    
    
}
ssl::~ssl() {
    //x509_free( &cacert );
    mbedtls_ssl_close_notify(&this->ctx);
    //net_close(this->sock);
    mbedtls_ssl_free(&this->ctx);
    free(this->request);
    free(this->reply);
  
  
}

bool ssl::open(const char *ip,int port) { 
    int handshake_status = 0, ctr_status=0;
    try {
  
      if(mbedtls_net_connect(&this->ctx_sock,ip,(const char *)port,MBEDTLS_NET_PROTO_TCP)!=0)
	throw 0;
     

      mbedtls_ssl_init(&this->ctx);
      mbedtls_ssl_config_init(&this->ssl_conf);
	
      
      mbedtls_ssl_conf_endpoint( &this->ssl_conf, SSL_IS_CLIENT );
      mbedtls_ssl_conf_authmode( &this->ssl_conf, SSL_VERIFY_OPTIONAL );
      mbedtls_ssl_conf_ca_chain( &this->ssl_conf, &this->cacert, NULL); // JennySSL
      mbedtls_ssl_conf_rng( &this->ssl_conf, ctr_drbg_random, &this->ctr_ctx );
      //mbedtls_set_dbg( &this->ctx, my_debug, stdout );
      mbedtls_ssl_set_bio( &this->ctx, (void *)&this->sock,net_send,net_recv, NULL );
    
      while((handshake_status=mbedtls_ssl_handshake(&this->ctx))!=0) {
	 if(handshake_status!= MBEDTLS_ERR_SSL_WANT_READ && handshake_status != MBEDTLS_ERR_SSL_WANT_WRITE )
	   throw 2;
      }
      
      
      //not available ssl cert integrity bug checking in v0.1 (added in v0.2 or v0.3)
      if((ctr_status=mbedtls_ssl_get_verify_result(&this->ctx))!=NULL) {
        if((ctr_status & BADCERT_EXPIRED ) != 0 )
            __logging("SSL: %s:%d Expired Cert\n",ip,port);
        if((ctr_status & BADCERT_REVOKED ) != 0 )
            __logging("SSL: %s:%d Revoked Cert\n",ip,port);
	
	/*temporary unavailable
        if((ctr_status & BADCERT_CN_MISMATCH ) != 0 )
            __logging("SSL: %s:%d CN mismath (expected CN=%s)\n", ip, port, "JennySSL" );
	*/
        //if((ctr_status & BADCERT_NOT_TRUSTED ) != 0 )
        //    __logging( "SSL: %s:%d untrusted or self-signed CA\n", ip, port );
	
      }
      
      return(true);
      
    }
    catch(int iResult) {
      switch(iResult) {
	case 0:
	  __logging("ssl::open: Connection error %s:%d\n",ip,port);
	  break;  
	case 1:
	  __logging("ssl::open: Error on init SSL\n");
	  break;
	case 2:
	  __logging("ssl::open: Error on SSL handshake (%d) %s:%d\n", handshake_status,ip,port);
	  break;
	default:
	  __logging("ssl::open: Unknow error %s:%d\n",ip,port);
	  break;
	
      }
      
      return(false);
    }
  
}
//int ssl::connect() { return 0; }
int ssl::send(const unsigned char *data,int len) { 
  int send_status = 0;
  
  try {
    bzero(this->request, MAX_SSL_BUFF);
    strlcpy((char *)this->request, (const char *)data, MAX_SSL_BUFF);
     
    if((send_status = mbedtls_ssl_write(&this->ctx, data, len))<=0) {
     if(send_status != MBEDTLS_ERR_SSL_WANT_READ && send_status != MBEDTLS_ERR_SSL_WANT_WRITE )
       throw 0;
    }
      
   return(send_status); 
    
  }
  catch(int iResult) {
    switch(iResult) {
      case 0:
	__logging("ssl::send: SSL write error %d\n", send_status);
	break;
      default:
	__logging("ssl::send: Unknow error\n");
	break;
    }
    
    
    return(send_status);
    
  }
}
int ssl::recv() { 
    int recv_status;
  try {
    /*
    if(len<MAX_SSL_BUFF)
      throw 2;
    */
    bzero(this->reply, MAX_SSL_BUFF);
    //strlcpy(this->reply, data, MAX_SSL_BUFF); s
//    data = this->reply;
    
    recv_status = mbedtls_ssl_read(&this->ctx, this->reply, MAX_SSL_BUFF);
    
     if(recv_status== MBEDTLS_ERR_SSL_WANT_READ || recv_status == MBEDTLS_ERR_SSL_WANT_WRITE)
	return 1;
     else if(recv_status == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
	throw 0;
     else if(recv_status < 0)
	throw 1;
     else if(recv_status==0)
       return 0;
     
     return(recv_status);
    
  }
  catch(int iResult) {
    
    switch(iResult) {
      case 0:
	__logging("ssl::recv: Error SSL notify session closed\n");
	break;
      case 1:
	__logging("ssl::recv: Error SSL read\n");
	break;
	/*
      case 2:
	__logging("ssl::recv: Max buff size is too long (%d bytes)", len);
	break;
	*/
      default:
	__logging("ssl::recv: Unknow error\n");
	break;
      
    }
    
    return 0;   
  }
  
}

/*
 * @note: Is slow change to own implementation of ssl port testing from jennyframework
*/
bool ssl::testport(char *host, unsigned short port,int proto) {
    try {
      if(proto!=0)
	throw 0;
      if(host==NULL)
	throw 1;
      if(port<=0 || port > 65535)
	throw 2;
     
     bool ishost = false;
     char *rhost = host;
     char *ip = NULL;
      
     if(netutils::isvalidIPV4(host)==false) {
	ip = (char *)__allocate(21);
	ishost=true;
	if(netutils::resolve(host,ip)==0) {
	    __liberty((void *)ip);
	    throw 3;
	}
	host=ip;
     }
      
     ssl *l_ssl = new ssl(); 
      bool ret = l_ssl->open(host,port);
      
      if(ishost==true) {
	
	if(ip!=NULL)
	  __liberty(ip);
	host = rhost;
      }
	
      delete l_ssl;
      return(ret);    
    }
    
    catch(int iResult) {
      switch(iResult) {
	case 0:
	  __logging("ssl::testport: Application in v0.1 only support detect SSL over tcp ports\n"); 
	  break;
	case 1:
	  __logging("ssl::testport: Error host is null\n"); 
	  break;
	case 2:
	  __logging("ssl::testport: Error invalid port %d\n",port); 
	  break;
	case 3:
	  __logging("ssl::testport: Error invalid host or IPV6 host %s:%d/(%d)\n",host,port);
	  break;
	default:
	  if(host)
	    __logging("ssl::testport: Unknow error %s:%d/(%d)\n",host,port,proto); 
	  else
	    __logging("ssl::testport: Unknow error null:%d/(%d)\n",port,proto); 
	  break;
      }
	
	return(false);
    }
}  


//I can get not sleep... sleep.. sleep.. Insomnia... 

