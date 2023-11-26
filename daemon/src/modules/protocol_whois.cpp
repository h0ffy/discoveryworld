#include <bsd/bsd.h>
#include <bsd/stdio.h>
#include "../misc.h"
#include "../net_utils.h"
#include "protocol_whois.h"

/* NON TESTED */

#define WHOIS_REFERRAL(host,whoip,type) { (char *)host, whoip, type }


static struct {
    char *host;
    int whoip;
    int type;
}whois_hosts[] {
  WHOIS_REFERRAL("whois.abuse.net",0,0),
  WHOIS_REFERRAL("whois.iana.org",0,0),
  WHOIS_REFERRAL("whois.ripe.net",1,0),
  WHOIS_REFERRAL("whois.nic.gov",0,0),
  WHOIS_REFERRAL("whois.arin.net",0,0),
  WHOIS_REFERRAL("whois.crsnic.net",0,0),
  WHOIS_REFERRAL("whois.networksolutions.com",0,0),
  WHOIS_REFERRAL("whois.lacnic.net",1,0),
  WHOIS_REFERRAL("whois.krnic.net",0,0),
  WHOIS_REFERRAL("whois.apnic.net",1,0),
  WHOIS_REFERRAL("whois.ra.net",0,0),
  WHOIS_REFERRAL(".whois-servers.net",0,1),
  WHOIS_REFERRAL("whois.registro.br",1,0),
  WHOIS_REFERRAL("whois.norid.no",0,0),
  WHOIS_REFERRAL("de.whois-servers.net",0,0),
  WHOIS_REFERRAL("whois.afrinic.net",1,0),
  {NULL,0,0},{NULL,0,0},{NULL,0,0},{NULL,0,0},
};


whois::whois() {
  try {
      this->reply = (char *)__allocate(MAX_WHOIS_REPLY);
    
  }
  catch(int iResult) {
    switch(iResult) {
      case 0:
	__logging("whois::whois: Critical memory error");
	break;
      default:
	__logging("whois::whois: Unknow error");
	break;
    }
  }
}

whois::~whois() {
  try {
      __liberty(this->reply);
    
  }
  catch(int iResult) {
     __logging("whois::~whois: Unknow error");
  }  
}

bool whois::query(char *data) {
 try{
    int whoip = 0, cur = 0;
    netutils *net = new netutils();
    
    
    if(net->isvalidIPV4(data)==true)
      whoip=1;
    
    
    while(whois_hosts[cur].host!=NULL || cur < MAX_WHOIS_SERVERS) {
      if(whoip==1 && whois_hosts[cur].whoip == 0) {
	cur++;
	continue;
      }
      else if(whois_hosts[cur].type == 1) {	// non implemented
	cur++;
	continue;
      }
      if(net->tcp_open(whois_hosts[cur].host,43)==1) {
	if(net->tcp_send(data,strlen(data))>1) {
	  if(net->tcp_recv(this->reply,MAX_WHOIS_REPLY)) {
	    break;
	  }
	  else {
	      __logging("whois::query: Whois reply error %s\n", whois_hosts[cur].host); 
	  }
	}
	else {
	  __logging("whois::query: Error on send data %s\n", whois_hosts[cur].host); 
	}
      }
      else {
	  __logging("whois::query: Invalid whois server %s\n", whois_hosts[cur].host);
      }
      
      net->tcp_close();
      cur++;
      
      delete net;
    }
    
 }
 catch(int iResult) {
  switch(iResult) {
    default:
      __logging("whois::query: Unknow error\n");
      break;
  }
 }
  
}



