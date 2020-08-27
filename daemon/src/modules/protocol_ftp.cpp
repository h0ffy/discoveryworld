#include <bsd/bsd.h>
#include <bsd/string.h>
#include <bsd/stdlib.h>
#include <bsd/stdio.h>
#include <bsd/string.h>
#include "../config.h"
#include "../misc.h"
#include "../net_utils.h"
#include "protocol_ftp.h"



ftp::ftp() {
  try {
    net = new netutils();
    this->last_node = 0;
    this->reply_status = false;
    if((this->reply=(char *)__allocate(MAX_FTP_REPLY))==NULL)
      throw 0;
    if((this->request=(char *)__allocate(MAX_FTP_REQUEST))==NULL)
      throw 0;
    
    bzero(this->request, MAX_FTP_REQUEST);
    bzero(this->reply, MAX_FTP_REQUEST);

  }
  catch(int iResult) {
      switch(iResult) {
	case 0:
	  __logging("ftp::ftp: Memory alloc error\n");
	  break;
	default:
	  __logging("ftp::ftp: Unknow error %x\n", iResult);
	  break;
      }
  }
}
ftp::~ftp() {
  try {
    
    do {
      if(this->nodes[this->last_node].status==true) {
	  __liberty(this->nodes[this->last_node].var);
	  __liberty(this->nodes[this->last_node].val);
      }
    }while(this->last_node--);
    
    __liberty(this->request);
    __liberty(this->reply);
    
    net->tcp_close();
    
  }
  catch(int iResult) {
      __logging("ftp::~ftp: Unknow error %x\n", iResult);
  }
 
  
}


/* non tested function */
bool ftp::finger(const char *ip, const int port) {
  try {
    if(this->open(ip,port)==false)
      return(false);
    
    this->request_add("SYST",NULL);
    this->build();
   
      
    return(this->reply_status);
  }
  catch(int iResult) {
    return(false);
  }
}

void ftp::request_init() {}


void ftp::request_make(int offset) {
  try {  
    if(offset<MAX_FTP_NODES)
      throw 0;
    if(this->nodes[offset].status==false)
      throw 1;
    
    bzero(this->request, MAX_FTP_REQUEST);
    
    strlcpy(this->request, this->nodes[offset].var, MAX_FTP_REQUEST);
    if(this->nodes[offset].val==NULL) {
      strlcat(this->request, FTP_EOL, MAX_FTP_REQUEST);
    }
    else {
      strlcat(this->request, (char *)" ", MAX_FTP_REQUEST);
      strlcat(this->request, this->nodes[offset].val, MAX_FTP_REQUEST);
      strlcat(this->request, FTP_EOL, MAX_FTP_REQUEST);
    }
    
  }
  catch(int iResult) {
    switch(iResult) {
      case 0:
	__logging("ftp::request_make: Max ftp nodes exceeded\n");
	break;
      case 1:
	__logging("ftp::request_make: Exception on read disable node %d\n", offset);
	break;
      default:
	__logging("ftp::request_make: Unknow error\n");
	break; 
    }
  }
  
}

void ftp::reply_add(const unsigned char *reply) {
    try{
  
      if(this->reply_status == false) {
	strlcpy(this->reply, (char *)reply, MAX_FTP_REPLY);
	strlcat(this->reply, "\n", MAX_FTP_REPLY);
	this->reply_status = true;
      }
      else
	strlcat(this->reply, (char *)reply, MAX_FTP_REPLY);
	strlcat(this->reply, "\n", MAX_FTP_REPLY);
    }
    catch(int iResult) {
	__logging("ftp::reply_add: Unknow error\n");
    }
  
}

void ftp::request_add(const char *var, const char *val) {
  try {
    if(this->last_node>MAX_FTP_NODES)
      throw 1;
    else if(strlen(var)>MAX_FTP_VAR)
      throw 2;
    else if(strlen(val)>MAX_FTP_VAL)
      throw 3;
    else if(var==NULL)
      throw 5;
    
    
    if((this->nodes[this->last_node].var = (char *)__allocate(MAX_FTP_VAR))==NULL)
      throw 4;
    if(val!=NULL) {
      if((this->nodes[this->last_node].val = (char *)__allocate(MAX_FTP_VAL))==NULL)
	throw 4;
      
      strlcpy(this->nodes[this->last_node].val, val, MAX_FTP_VAL);
    }
    else 
      this->nodes[this->last_node].val = NULL;

    strlcpy(this->nodes[this->last_node].var, var, MAX_FTP_VAR);
    this->nodes[this->last_node].status = true;
    this->last_node++;
    
  }
  catch(int iResult) {
    switch(iResult) {
      case 1:
	 __logging("ftp::request_add: Not allow more nodes\n");
	 break;
      case 2:
	 __logging("ftp::request_add: Var buff is too long\n");
	 break;
      case 3:
	 __logging("ftp::request_add: Val buff is too long\n");
	 break;
      case 4:
	 __logging("ftp::request_add: Allocation memory error\n");
	 break;
      case 5:
	 __logging("ftp::request_add: Invalid add null request\n");
	 break;
      default:
	__logging("ftp::request_add: Unknow error\n");
	break;
    }
  }
}

bool ftp::open(const char *ip, const int port) {
  try {
      if(net->tcp_open(ip,port)==false)
	throw 0;
      
      return(true);
  }
  catch(int iResult) {
      switch(iResult) {
	case 0:
	  __logging("ftp::open: Connection error\n");
	  break;
	default:
	  __logging("ftp::open: Unknow error\n");
	  break;
      }
      return(false);
  }
}





void ftp::build() {
    int i = 0;
    char *tmp_reply;
  try {
    if((tmp_reply=(char *)__allocate(MAX_FTP_REPLY))==NULL)
      throw 0;
    
    for(i=0;i<=this->last_node;i++) {
      if(this->nodes[i].status==true)
	this->request_make(i);
      else
	continue;
      
      if(net->tcp_recv(tmp_reply, MAX_FTP_REPLY)>0) {
	if(tmp_reply && strlen(tmp_reply)>1)
	  this->reply_add((const unsigned char *)tmp_reply);
      }
	
      if(net->tcp_send(this->request,strlen(this->request))!=0) {
	bzero(tmp_reply, MAX_FTP_REPLY);
	if(net->tcp_recv(tmp_reply, MAX_FTP_REPLY)>0) {
	  if(tmp_reply && strlen(tmp_reply)>1)
	    this->reply_add((const unsigned char *)tmp_reply);
	} 
      }
    } //end of for
    
    __liberty(tmp_reply);
  }
  catch(int iResult) {
    switch(iResult) {
      case 0:
	__logging("ftp::build: Alloc memory error\n");
	break;
      default:
	__logging("ftp::build: Unknow Error\n");
	break;
    }
  }
}