#include <bsd/bsd.h>
#include <bsd/string.h>
#include <bsd/stdlib.h>
#include <bsd/stdio.h>
#include <bsd/string.h>
#include <mbedtls/base64.h>
#include <assert.h>
#include "../config.h"
#include "../misc.h"
#include "../net_utils.h"
#include "protocol_smtp.h"

#define SMTP_HELO 0
#define SMTP_EHLO 1
#define SMTP_STARTTLS 2
#define SMTP_HELP 3

#define SMTP_REFERRAL(cid,command,isbase64,isparams) {cid,command,isbase64,isparams}

static struct {
    int cid; //command id
    const char *command;
    bool isbase64;
    bool isparams;
}smtp_commands[] {
    SMTP_REFERRAL(SMTP_HELO,"HELO",false,true),
    SMTP_REFERRAL(SMTP_EHLO,"EHLO",false,false),
    SMTP_REFERRAL(SMTP_STARTTLS,"STARTTLS", false, false),
    SMTP_REFERRAL(SMTP_HELP,"HELP", false, false),
    {-1,NULL,false,false},
};


smtp::smtp() {
  this->net = new netutils();
  this->request=(char *)__allocate(SMTP_MAX_BUFF);
  this->reply=(char *)__allocate(SMTP_MAX_BUFF);
  this->reply_status=false;
  
  
}
smtp::~smtp() {
  if(this->request)
    __liberty(this->request);
  if(this->reply)
    __liberty(this->reply);
  
  delete(this->net);
}


bool smtp::open(const char *host, unsigned short port) {
  return(this->net->tcp_open(host,port)); 
}

int smtp::send() {
  //bzero(this->request, SMTP_MAX_BUFF);
  return(this->net->tcp_send(this->request,strlen(this->request)));
}

int smtp::recv() {
 // bzero(this->reply, SMTP_MAX_BUFF);
  return(this->net->tcp_recv(this->reply, SMTP_MAX_BUFF));
}

void smtp::helo(const char *host) {
  bzero(this->request, SMTP_MAX_BUFF);
  sprintf(this->request, "%s %s\r\n", smtp_commands[SMTP_HELO].command,host);
}
void smtp::ehlo(const char *host) {
  bzero(this->request, SMTP_MAX_BUFF);
  sprintf(this->request, "%s %s\r\n", smtp_commands[SMTP_EHLO].command,host);
}
void smtp::starttls() {
  bzero(this->request, SMTP_MAX_BUFF);
  strlcpy(this->request, smtp_commands[SMTP_STARTTLS].command,SMTP_MAX_BUFF);
  strlcat(this->request, SMTP_EOL, SMTP_MAX_BUFF);
}

void smtp::help() {
  bzero(this->request, SMTP_MAX_BUFF);
  strlcpy(this->request, smtp_commands[SMTP_HELP].command,SMTP_MAX_BUFF);
  strlcat(this->request, SMTP_EOL, SMTP_MAX_BUFF);
}


bool smtp::auth(const char *user,const char *password,bool isbase64) {
  return(true);
}


void smtp::addreply(const char *buff) {
  try {
    
      if(strlen(buff)>SMTP_MAX_BUFF)
	throw 0;
    
      if(this->reply_status==true) {
	strlcat(this->reply,buff, SMTP_MAX_BUFF);
      }
      
      else {
	if((strlen(buff)+strlen(this->reply))>SMTP_MAX_BUFF)
	  throw 1;
	
	strlcpy(this->reply,buff,SMTP_MAX_BUFF);
	this->reply_status=true;
      } 
      
      
  }
  catch(int iResult) {
    switch(iResult) {
      case 0:
	__logging("smtp::addreply: Buff is too long %d\n", strlen(buff));
	break;
      case 1:
	__logging("smtp::addreply: Buff exceeded %d\n", strlen(buff)+strlen(this->reply));
	break;
      default:
	__logging("smtp::addreply: Unknow error\n");
	break;
    }
  }
}


//finger by one call
bool smtp::finger(const char *host, unsigned short port) {
  try {
    char *l_buff=(char *)__allocate(SMTP_MAX_BUFF);
    
    if(this->open(host,port)==false)
      throw 0;
    
    if(this->net->tcp_recv(l_buff, SMTP_MAX_BUFF)>1) {
      this->addreply(l_buff);
      bzero(l_buff,SMTP_MAX_BUFF);
    }

        
    this->ehlo(host);
    if(this->send()>1) {
	if(this->net->tcp_recv(l_buff,SMTP_MAX_BUFF)>1) {
	  this->addreply(l_buff);
	  bzero(l_buff,SMTP_MAX_BUFF);
	}      
    }
    
    this->helo(host);
    
    if(this->send()>1) {
	while(this->net->tcp_recv(l_buff,SMTP_MAX_BUFF)>1) {
	  this->addreply(l_buff);
	  bzero(l_buff,SMTP_MAX_BUFF);
	}      
    }

        
    this->starttls();
    
    if(this->send()>1) {
	while(this->net->tcp_recv(l_buff,SMTP_MAX_BUFF)>1) {
	  this->addreply(l_buff);
	  bzero(l_buff,SMTP_MAX_BUFF);
	}      
    }
        
   this->help();
    
    if(this->send()>1) {
	while(this->net->tcp_recv(l_buff,SMTP_MAX_BUFF)>1) {
	  this->addreply(l_buff);
	  bzero(l_buff,SMTP_MAX_BUFF);
	}      
    }

    __liberty(l_buff);
    delete(net);
    return(true);
  }
  catch(int iResult) {
    switch(iResult) {
      case 0:
	__logging("smtp::finger: Connection error %s:%d\n", host,port);
	break;
      default:
	__logging("smtp::finger: Unknow error\n");
	break;
    }
    return(false);
  }  
}


void smtp::close() {
  this->net->tcp_close();
}
